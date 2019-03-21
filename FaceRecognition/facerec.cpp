#include "facerec.h"
#include "ui_facerec.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

FaceRec::FaceRec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaceRec)
{
    ui->setupUi(this);
    // read an image,Mat是基础图像容器
    cv::Mat image = cv::imread("D:/test.jpg", 1);
    // create image window named "My Image"
    cv::namedWindow("My Image");
    // show the image on window
    cv::imshow("My Image", image);

}

FaceRec::~FaceRec()
{
    delete ui;
}
