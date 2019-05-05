#ifndef IMGTHREAD_H
#define IMGTHREAD_H

#include <QThread>
class imgThread : public QThread
{
    Q_OBJECT
public:
    explicit imgThread(QObject * parent = 0);
    void stop();

protected:
    void run();

private:
    volatile bool stopped;
};

#endif // IMGTHREAD_H
