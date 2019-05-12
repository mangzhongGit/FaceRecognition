#ifndef FACEREC_H
#define FACEREC_H
#include <headers.h>
#include <takephoto.h>
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

private slots:
    void on_localImgButton_clicked();       //获取本地图像

    void read_frame();   //读取当前帧信息

    void on_openCameraButton_clicked();     //打开摄像头

    void on_takePhotoButton_clicked();      //拍照

    void on_closeCameraButton_clicked();    //关闭摄像头

    void reshow();

    void on_fillLightButton_clicked();

    void on_takeOwnPhotoButton_clicked();

    void on_demoButton_clicked();

    void on_testButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::FaceRec *ui;
    QTimer *timer;
    VideoCapture cap; //视频获取结构， 用来作为视频获取函数的一个参数
    Mat frame;        //Mat类型，每一帧存放地址
    QString filename;
    QImage *img;
    Mat imgMat;

    Mat demoMat;
    Mat testMat;

    Ptr<EigenFaceRecognizer> model;

    int arrayColor[256];    // 补光提亮映射表

    Mat fill_light(Mat InputMat);            //补光操作

    Mat auto_fill_light(Mat InputMat);       //自动补光操作

    Mat auto_adjust_light(Mat InputMat);     //自动调整光照

    int judge_light_intensity(Mat InputMat); //判断光照强度

    Mat balance_white(Mat InputMat);          // 灰色世界法白平衡

    void model_detect(Mat imgMat);            // 模型检测

    void get_chart();

    float get_brightness(Mat InputMat);

};

#endif // FACEREC_H
