#ifndef IMGTHREAD_H
#define IMGTHREAD_H
#include <headers.h>
class takePhotoThread : public QThread
{
    Q_OBJECT
public:
    explicit takePhotoThread(QObject * parent = 0 );
    void stop();

protected:
    void run();

private:
    volatile bool stopped;
};

#endif // IMGTHREAD_H
