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

class MultiThread : public QThread
{
public:
    MultiThread(QDir filename, std::string winName, int cam, std::atomic<bool> *recordCheck);
    ~MultiThread();

    void run() override
    {
       record_cam();

    }

    void record_cam();


private:

    QDir m_filename;
    std::string m_winName;
    int m_cam;
    std::atomic<bool> *m_recordChecked;

    cv::VideoCapture *videoCam;
    cv::VideoWriter *video;

private slots:

};

#endif // MULTITHREAD_H
