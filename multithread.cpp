#include "multithread.h"

MultiThread::MultiThread(QDir filename, std::string winName, int cam, std::atomic<bool> *recordCheck) :
    m_filename(filename), m_winName(winName), m_cam(cam), m_recordChecked(recordCheck)
{
}

void MultiThread::record_cam()
{
    videoCam = new cv::VideoCapture(m_cam);
//    videoCamopen(m_cam); //Open cam

//    cv::VideoWriter video; //Class to write the frames
    video = new cv::VideoWriter();

    int fourcc = cv::VideoWriter::fourcc('M','J','P','G'); //Set forcc variable

    cv::Mat frame; //Frame of video
    int fps = 0; //Fps

    //Check if cam open
    if(!videoCam->isOpened()){
        std::cerr<<"Cannot open VideoCaputre cam "+ m_cam;
        return;
    }

    QString videoFilename = m_filename.absolutePath();

    videoCam->grab(); //To get fps value
    videoCam->retrieve(frame);
    fps = videoCam->get(cv::CAP_PROP_FPS);

        cv::namedWindow(m_winName,cv::WINDOW_NORMAL); //Window

    if(m_winName=="Principal")
        cv::resizeWindow(m_winName, cv::Size(640,480));//Principal windowSize
    else
        cv::resizeWindow(m_winName, cv::Size(640,480));//Auxiliar windowSize

    video->open(videoFilename.toStdString(), fourcc, fps, frame.size(), true);

    if(!video->isOpened())
    {
        std::cerr<<"Cannot open VideoWriter video";
        return;
    }

    while(videoCam->read(frame))
    {
        video->write(frame);

        cv::imshow(m_winName,frame);

        if(*m_recordChecked == true)
        {
//            *m_recordChecked = true;//turn off button
            break;
        }
    }

    videoCam->release();
    video->release();

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiThread::~MultiThread()
{
    delete video;
    delete videoCam;
}
