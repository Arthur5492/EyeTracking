#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <QThread>
#include <QString>
#include <iostream>
#include <string>
#include <QDir>
#include<opencv2/opencv.hpp>
#include <atomic>
#include <QApplication>
#include <QMutex>
#include <time.h>
#include <QWaitCondition>
class MultiThread : public QThread
{
public:
    MultiThread(std::string winName, int cam, std::atomic<bool> *checked, std::atomic<bool> *startRecord);
    ~MultiThread();

    void run() override
    {
       record_cam();

    }

    void record_cam();

    //bool get_setCamerastoRecord(){ return m_setCamerastoRecord; }

    void setFileName(QDir filename);
    void setCam(int camera);

private:

    QDir m_filename; //Filename .avi
    std::string m_winName;//Opencv window name
    int m_cam; //Cam to record

    QMutex mutex;
    //QWaitCondition waitCondition; future

    std::atomic<bool> *m_recordChecked; //Pause stop variable
    std::atomic<bool> *m_startRecord; // Sync variable

    //bool m_setCamerastoRecord;

    cv::VideoCapture *videoCam;
    cv::VideoWriter *video;

private slots:

signals:
//    void sendFrame(std::string winName, cv::Mat frame);

};

#endif // MULTITHREAD_H
