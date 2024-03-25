#include "multithread.h"

MultiThread::MultiThread(std::string winName, int cam, std::atomic<bool> *checked, std::atomic<int> *waitSync, cv::Mat frame) :
    m_winName(winName), m_cam(cam), m_recordChecked(checked), m_waitSync(waitSync), m_frame(frame)
{
    m_fps    =
    width    =
    height   = 0;
    updateGUI = false;
}

void MultiThread::record_cam()
{
    if(m_cam == -1)
        return;

    updateGUI = false;
    int fourcc = cv::VideoWriter::fourcc('M','J','P','G'); //Set forcc variable



    if(m_winName =="Principal")//800x600 in future
    {
        width = 800;
        height = 600;
    }
    else if(m_winName =="Auxiliar")
    {
        width = 640;
        height = 480;
    }

    videoCam.set(cv::CAP_PROP_FRAME_WIDTH,width);
    videoCam.set(cv::CAP_PROP_FRAME_HEIGHT,height);

    if(videoCam.isOpened() == false){
        #if (defined (LINUX) || defined (__linux__) )
          videoCam.open(m_cam, cv::CAP_V4L2);
        #elif (defined (_WIN32) || defined (_WIN64))
          videoCam.open(m_cam, cv::CAP_DSHOW);
        #endif
    }

    if(!videoCam.isOpened()){
        std::cerr<<"Cannot open VideoCapture cam " + std::to_string(m_cam);
        emit returnIfNoCam();
        return;
    }
    else
        videoCam.read(m_frame);

    cv::VideoWriter video;

    std::string tempFile = m_filename.toStdString()+ "temp.avi";
    video.open(tempFile, fourcc, 1 , m_frame.size(), true);

    if(!video.isOpened())
    {
        std::cerr<<"Cannot open VideoWriter video";
        return;
    }

    (*m_waitSync)++;

    while(*m_waitSync != 2)
    {
        if(m_recordChecked->load()==false)
        {
            if(videoCam.isOpened())
                videoCam.release();
            if(video.isOpened())
                video.release();
            return;
        }

    }

    emit sendWindow();

    std::cerr<<m_winName + " Entering thread"<<std::endl;

    time_t start,end;

    time(&start);

    while(videoCam.read(m_frame) && m_recordChecked->load()==true)
    {
        video.write(m_frame);
        emit sendFrame(m_winName, m_frame);
    }

    time(&end);

    seconds = difftime(end, start);

    video.release();
    videoCam.release();

    //Not working
    //emit sendFinishRecord();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    configureCaption(tempFile, m_filename.toStdString() ,fourcc);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MultiThread::setCam(int camera)
{
    QMutexLocker locker(&mutex);//Data race
    if(m_cam != camera)
        m_cam = camera;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MultiThread::setFileName(QString filename)
{
    QMutexLocker locker(&mutex);//Data race
    m_filename = filename;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MultiThread::configureCaption(std::string tempFile, std::string newFile ,int fourcc)
{
    QFile temporaryFile(QString::fromStdString(tempFile));

    cv::VideoCapture oldVideo(tempFile);
    int frameCount = oldVideo.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = frameCount/seconds;

    cv::VideoWriter video(newFile, fourcc, fps, m_frame.size(),true);

    for(int i = 0; i<frameCount; i++)
    {
        bool haveFrame = oldVideo.read(m_frame);
        if(haveFrame)
            video.write(m_frame);
        else
            break;
    }

    oldVideo.release();


    temporaryFile.remove();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiThread::~MultiThread()
{
}
