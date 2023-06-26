#ifndef RECORDTHREAD_H
#define RECORDTHREAD_H

#include <QMainWindow>
#include <iostream>
#include <QThread>
#include "mainwindow.h"

class RecordThread : public QThread{
    Q_OBJECT

public:
    RecordThread(const QString filename, const std::string& cameraName);

protected:
    void run() override;

private:
    QString filename;
    std::string cameraName;
};

#endif // RECORDTHREAD_H
