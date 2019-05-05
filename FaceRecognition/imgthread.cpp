#include <imgthread.h>

imgThread::imgThread(QObject *parent):
    QThread (parent)
{
    stopped = false;
}

void imgThread::run()
{

    stopped = false;
}

void imgThread::stop()
{
    stopped = true;
}


