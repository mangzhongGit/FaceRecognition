#ifndef FACEREC_H
#define FACEREC_H
#include <headers.h>
#include <takephoto.h>
#include <tools.h>
namespace Ui {
class FaceRec;
}
class myThread;
class FaceRec : public QWidget
{
    Q_OBJECT

public:
    explicit FaceRec(QWidget *parent = nullptr);
    ~FaceRec();

signals:
    void sendMat(Mat frame, int judgeMe);
    void sendFlag(int flag);

private slots:
    void on_localImgButton_clicked();       //获取本地图像

    void read_frame();   //读取当前帧信息

    void on_openCameraButton_clicked();     //打开摄像头

    void reshow();

    void receiceSignal(int predictPCA, Mat face, double confidence);

    void receiveFlag(int flag);

    void on_takeOwnPhotoButton_clicked();

    void on_fillBrightButton_clicked();

    void on_nonLineButton_clicked();

    void on_faceEnButton_clicked();

    void on_modelTrainButton_clicked();

    void on_loadXMLButton_clicked();

    void on_gammaButton_clicked();

    void on_improveButton_clicked();

    void stopRec();

    void on_pushButton_clicked();

private:
    Ui::FaceRec *ui;
    QTimer *timer;
    VideoCapture cap; //视频获取结构， 用来作为视频获取函数的一个参数
    Mat frame;        //Mat类型，每一帧存放地址
    QString filename;
    QImage *img;
    Mat imgMat;

    int judgeMe = 0;
    myThread *thread;
    Ptr<EigenFaceRecognizer> model;

    QString get_information(Mat InputMat);
    void closeCamera();    //关闭摄像头

    QSqlQuery *query;
    QTime startTime, stopTime;
    QTimer *recTimer;

    takePhoto *photo;
};

#endif // FACEREC_H
