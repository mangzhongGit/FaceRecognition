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
    void receiveMat(Mat frame, int judgeMe);
    void receiveFlag(int flag);

signals:
    void sendsignal(int predictPCA,Mat face, double confidence);
    void sendFlag(int flag);

protected:
    void run();
    vector<Mat> frameMat;
    Mat frame0;
    int flag0 = 0;
    int flag = 0;
    Ptr<EigenFaceRecognizer> model;

private:
    volatile bool stopped;
};

#endif // IMGTHREAD_H
