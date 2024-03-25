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
#include <chrono>
#include <thread>
class MultiThread : public QThread
{
    Q_OBJECT

public:
    MultiThread(std::string winName, int cam, std::atomic<bool> *checked, std::atomic<int> *waitSync, cv::Mat frame);
    ~MultiThread();

    bool updateGUI;

    void run() override
    { record_cam(); }
//    void configureCaption(std::string fileName, int fourcc);

    //Configura e grava
    void record_cam();

    //Regrava o video com fps correto
    void configureCaption(std::string tempFile, std::string newFile, int fourcc);


    //Setters
    void setFileName(QString filename);
    void setCam(int camera);

private:

    //given parameters
    QString m_filename; //Filename .avi
    std::string m_winName;//Opencv window name
    int m_cam; //Cam to record
    cv::Mat m_frame; //Frame of video

    QMutex mutex;//Local Mutex???

    std::atomic_bool *m_recordChecked; //Pause stop variable
    std::atomic<int> *m_waitSync; // Sync variable

    int m_fps;

    int width;
    int height;

//    Para guardar o tempo de gravacao
    double seconds;

    //Camera
    cv::VideoCapture videoCam;

private slots:

signals:
    //Envia cv::Mat frame para o main, da erro imshow em qthread
    void sendFrame(std::string winName, const cv::Mat frame);

    void sendWindow();

    void sendFinishRecord();

    void returnIfNoCam();

};

#endif // MULTITHREAD_H
