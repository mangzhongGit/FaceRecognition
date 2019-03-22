#ifndef FACEREC_H
#define FACEREC_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
using namespace cv;

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

private:
    Ui::FaceRec *ui;

    QImage Mat2QImage(const cv::Mat& InputMat);   //cv::Mat类型转换为QImage类型
    QTimer *timer;
    VideoCapture cap; //视频获取结构， 用来作为视频获取函数的一个参数
    Mat frame;        //Mat类型，每一帧存放地址

    QImage *img;
    QString filename;

};

#endif // FACEREC_H
