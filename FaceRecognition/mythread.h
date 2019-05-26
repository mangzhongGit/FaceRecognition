#ifndef IMGTHREAD_H
#define IMGTHREAD_H
#include <headers.h>
#include <tools.h>
class myThread : public QThread
{
    Q_OBJECT
public:
    explicit myThread(QObject * parent = 0 );
    void stop();

private slots:
    void receiveMat(Mat frame);
    void receiveFlag(int flag);

signals:
    void sendsignal();

protected:
    void run();
    Mat frame0;
    Ptr<EigenFaceRecognizer> model0;
    int flag = 0;
    Ptr<EigenFaceRecognizer> model;

private:
    volatile bool stopped;
};

#endif // IMGTHREAD_H
