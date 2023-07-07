#include "multithread.h"

MultiThread::MultiThread(std::string winName, int cam, std::atomic<bool> *checked, std::atomic<int> *waitSync, int fps) :
    m_winName(winName), m_cam(cam), m_recordChecked(checked), m_waitSync(waitSync), m_fps(fps)
{
    videoCam = nullptr;
    video    = nullptr;
}

void MultiThread::record_cam()
{
    QMutexLocker locker(&mutex);//Data race


    //Can be 'M','J','P','G'
    int fourcc = cv::VideoWriter::fourcc('X','V','I','D'); //Set forcc variable

    if(m_cam == -1)
        return;

    if(!videoCam)
        videoCam = new cv::VideoCapture(m_cam, cv::CAP_V4L2);//cv::CAP_V4L2

    int width = 0;//cv::CAP_PROP_FRAME_WIDTH;
    int height = 0;//cv::CAP_PROP_FRAME_HEIGHT;

    if(m_winName =="Principal")//800x600 in future
    {
        width = 640;
        height = 480;
    }
    else if(m_winName =="Auxiliar")
    {
        width = 640;
        height = 480;
    }

    videoCam->set(cv::CAP_PROP_FRAME_WIDTH,width);
    videoCam->set(cv::CAP_PROP_FRAME_HEIGHT,height);



    videoCam->read(frame);
    int fps = int(videoCam->get(cv::CAP_PROP_FPS));
    if(m_winName == "Principal")
        fps = 15;

    if(!videoCam->isOpened()){
        std::cerr<<"Cannot open VideoCapture cam "+ m_cam;
        return;
    }

    if(!video)
        video = new cv::VideoWriter();

    QString videoFilename = m_filename.absolutePath();

    video->open(videoFilename.toStdString(), fourcc, fps, frame.size(), true);

    std::cerr<<m_winName+":"<<fps<<std::endl;

    if(!video->isOpened())
    {
        std::cerr<<"Cannot open VideoWriter video";
        return;
    }

    locker.unlock();

    (*m_waitSync)++;

    while(*m_waitSync != 2)
        ;

    std::cerr<<m_winName + " Entering thread"<<std::endl;

//    cv::Mat P_frameCopy = frame.clone();

    while(videoCam->read(frame) && *m_recordChecked)
    {
        if(frame.empty() == false)
        {
            video->write(frame);
            emit sendFrame(m_winName, frame);
            frame.release();
        }
    }

    video->release();//Release videoWriter, but VideoCam still
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MultiThread::setCam(int camera)
{
    QMutexLocker locker(&mutex);//Data race
    if(camera != m_cam)
    {
        //If cam is opened
        if(videoCam)
        {
            videoCam->release();
            videoCam = nullptr;
        }
        //Change cam value
        m_cam = camera;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MultiThread::setFileName(QDir filename)
{
    QMutexLocker locker(&mutex);//Data race
    m_filename = filename;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiThread::~MultiThread()
{
    //Delete videoWriter and VideoCapture
    if(videoCam)
        delete videoCam;
    if(video)
        delete video;
}
