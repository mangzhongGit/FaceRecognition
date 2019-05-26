#ifndef FACEREC_H
#define FACEREC_H
#include <headers.h>
#include <takephoto.h>
#include <mythread.h>
#include <tools.h>
namespace Ui {
class FaceRec;
}

class FaceRec : public QWidget
{
    Q_OBJECT

public:
    explicit FaceRec(QWidget *parent = nullptr);
    ~FaceRec();

signals:
    void sendMat(Mat frame);
    void sendFlag(int flag);

private slots:
    void on_localImgButton_clicked();       //获取本地图像

    void read_frame();   //读取当前帧信息

    void on_openCameraButton_clicked();     //打开摄像头

    void on_takePhotoButton_clicked();      //拍照

    void on_closeCameraButton_clicked();    //关闭摄像头

    void reshow();

    void receiceSignal();

    void on_takeOwnPhotoButton_clicked();

    void on_fillBrightButton_clicked();

    void on_nonLineButton_clicked();

    void on_faceEnButton_clicked();

    void on_modelTrainButton_clicked();

    void on_loadXMLButton_clicked();

private:
    Ui::FaceRec *ui;
    QTimer *timer;
    VideoCapture cap; //视频获取结构， 用来作为视频获取函数的一个参数
    Mat frame;        //Mat类型，每一帧存放地址
    QString filename;
    QImage *img;
    Mat imgMat;

    int predictPCA;
    double confidence;
    int judgeMe;
    myThread *thread;

    float demoBright;

    Ptr<EigenFaceRecognizer> model;

    int arrayColor[256];    // 补光提亮映射表

    Mat fill_light(Mat InputMat);            //补光操作

    Mat auto_fill_light(Mat InputMat);       //自动补光操作

    Mat auto_adjust_light(Mat InputMat);     //自动调整光照

    int judge_light_intensity(Mat InputMat); //判断光照强度

    Mat balance_white(Mat InputMat);          // 灰色世界法白平衡

    void get_chart();

    float get_brightness(Mat InputMat);

    Mat gamma_correct(Mat InputMat, float fGamma); //gamma校验

    Mat enhanced_local_texture_feature(Mat  InputMat); //增强的局部纹理特征集，在困难的灯光下进行人脸识别

    QString get_information(Mat InputMat);
};

#endif // FACEREC_H
