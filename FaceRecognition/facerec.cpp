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
    for(int i=0; i<256; i++)
    {
        if((i % 3) == 0) arrayColor[i] = i;
        else if((i % 3) == 1) arrayColor[i] = i+1;
        else arrayColor[i] = i+2;
        if(arrayColor[i]>255) arrayColor[i]=255;
    }

    ui->label->setFixedSize(600, 800);
    ui->label->setText("用于显示原图");
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label_2->setFixedSize(600, 800);
    ui->label_2->setText("用于显示处理后的效果图");
    ui->label_2->setAlignment(Qt::AlignCenter);
    // XML文件即是我们人脸检测所需要的分类器文件
    xmlpath = "D:\\opencv-3.4.5\\opencv-3.4.5-build"
              "\\install\\etc\\haarcascades\\haarcascade_frontalface_default.xml";

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
        // *img = recognize_face(*img);
        //让图片自适应label的大小
        QPixmap *pixmap = new QPixmap(QPixmap::fromImage(*img));
        pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
        ui->label->setScaledContents(true);
        ui->label->setPixmap(*pixmap);
    }
}

//打开摄像头
void FaceRec::on_openCameraButton_clicked()
{
    cap = NULL;
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
    *img = recognize_face(*img);
    ui->label->setPixmap(QPixmap::fromImage(*img));  //将图片显示到label上
}

//拍照
void FaceRec::on_takePhotoButton_clicked()
{
    cap>>frame;
    *img = Mat2QImage(frame);
    ui->label_2->setPixmap(QPixmap::fromImage(*img));
}

//关闭摄像头
void FaceRec::on_closeCameraButton_clicked()
{
    timer->stop();
    cap.release();
}

// 补光按钮操作
void FaceRec::on_fillLightButton_clicked()
{
    Mat middle = QImage2cvMat(*img);
//    QImage test = Mat2QImage(fill_light(middle));
//    ui->label_2->setPixmap(QPixmap::fromImage(test));
    auto_adjust_light(middle);
}
