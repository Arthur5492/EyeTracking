#include "recordthread.h"

RecordThread::RecordThread(const QString filename, const std::string& cameraName)
    : filename(filename), cameraName(cameraName){}

void RecordThread::run()
{
    MainWindow* window;
    window->record_cam(filename, cameraName);
}
