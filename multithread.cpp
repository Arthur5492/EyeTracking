#include "multithread.h"

MultiThread::MultiThread(std::string winName, int cam, std::atomic<bool> *checked, std::atomic<bool> *startRecord) :
             m_winName(winName), m_cam(cam), m_recordChecked(checked), m_startRecord(startRecord)
{
    videoCam = nullptr;
    video=nullptr;
}

void MultiThread::record_cam()
{
    QMutexLocker locker(&mutex);//Data race
    cv::Mat frame; //Frame of video

    int fourcc = cv::VideoWriter::fourcc('M','J','P','G'); //Set forcc variable

    if(m_cam == -1)
        return;

    if(!videoCam)
        videoCam = new cv::VideoCapture(m_cam, cv::CAP_V4L2);//cv::CAP_V4L2

//    videoCam->read(frame);
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
    int fps = videoCam->get(cv::CAP_PROP_FPS);

    if(!videoCam->isOpened()){
        std::cerr<<"Cannot open VideoCapture cam "+ m_cam;
        return;
    }

    if(!video)
        video = new cv::VideoWriter();

    QString videoFilename = m_filename.absolutePath();

    video->open(videoFilename.toStdString(), fourcc, 30, frame.size(), true);

    std::cerr<<m_winName+":"<<fps<<std::endl;

    if(!video->isOpened())
    {
        std::cerr<<"Cannot open VideoWriter video";
        return;
    }

//    cv::namedWindow(m_winName,cv::WINDOW_NORMAL); //Window

    locker.unlock();

//    m_winName=="Principal" ?
//        cv::resizeWindow(m_winName, cv::Size(640,480)) :
//        cv::resizeWindow(m_winName, cv::Size(640,480)) ;

    std::cerr<<m_winName + " Entering thread"<<std::endl;

    //Gambiarra, pretendo usar QWaitCondition
    if(m_winName == principal)
        *m_startRecord = true;
    // gambiarra
    while(!(*m_startRecord))
        ;

      //Frame copies
//    cv::Mat A_frameCopy = frame.clone();
//    cv::Mat P_frameCopy = frame.clone();

    while(videoCam->read(A_frameCopy))
    {
        if(*m_recordChecked == false)//if record button clicked
        break;
        video->write(A_frameCopy);
//        emit sendFrame(m_winName, frame);
//        cv::imshow(m_winName,frameCopy);
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
