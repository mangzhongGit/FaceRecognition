#include "facerec.h"
#include "ui_facerec.h"

FaceRec::FaceRec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaceRec)
{
    ui->setupUi(this);
    //初始化
    timer = new QTimer(this);
    img = new QImage();
    cap = NULL;

    //信号和槽
    connect(timer, SIGNAL(timeout()), this, SLOT(read_frame()));  //时间到，读取当前摄像头

}

FaceRec::~FaceRec()
{
    delete img;
    delete ui;
}

//获取本地图像
void FaceRec::on_localImgButton_clicked()
{
    filename = QFileDialog::getOpenFileName(this,
                                            tr("选择本地图像"),
                                            "",
                                            tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        if(! (img->load(filename))) //加载图像
        {
            QMessageBox::warning(this,
                                     tr("图像打开失败"),
                                     tr("图像打开失败"));
            return;
        }
        ui->label->setPixmap(QPixmap::fromImage(*img));
    }
}

//打开摄像头
void FaceRec::on_openCameraButton_clicked()
{
    cap.open(0);
    if(!cap.isOpened())
    {
        QMessageBox::warning(this,
                             tr("frame is empty"),
                             tr("frame is empty"));
    }
    timer->start(40);   //开始计时，不可缺少，控制帧率，在这里为25帧
}

//读取当前帧信息
void FaceRec::read_frame()
{
    cap>>frame; //从摄像头中抓取并返回每一帧

    //将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式
    *img = Mat2QImage(frame);
    ui->label->setPixmap(QPixmap::fromImage(*img));  //将图片显示到label上
}

//拍照
void FaceRec::on_takePhotoButton_clicked()
{
    cap>>frame;
    *img = Mat2QImage(frame);
    ui->label->setPixmap(QPixmap::fromImage(*img));
}

//关闭摄像头
void FaceRec::on_closeCameraButton_clicked()
{
    timer->stop();
    cap.release();
}

// cv::Mat类型转换为QImage类型
QImage FaceRec::Mat2QImage(const cv::Mat& InputMat)
{
    cv::Mat TmpMat;
    //将颜色空间转换为RGB
    if (InputMat.channels() == 1)
    {
        cv::cvtColor(InputMat, TmpMat, CV_GRAY2RGB);
    }
    else
    {
        cv::cvtColor(InputMat, TmpMat, CV_BGR2RGB);
    }
    //使用mat的数据构造qimage，而不复制数据
    QImage Result = QImage(static_cast<const uchar*>(TmpMat.data),
                           TmpMat.cols,TmpMat.rows, QImage::Format_RGB888);
    //深拷贝数据从Mat到QImage
    Result.bits();
    return Result;
}
