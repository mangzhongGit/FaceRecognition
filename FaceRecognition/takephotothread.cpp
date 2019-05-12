#include <takephotothread.h>

takePhotoThread::takePhotoThread(QObject *parent):
    QThread (parent)
{
    stopped = false;
}

void takePhotoThread::run()
{
    stopped = false;
}

void takePhotoThread::stop()
{
    stopped = true;
}
