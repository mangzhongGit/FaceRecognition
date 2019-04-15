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
        ui->label->setPixmap(QPixmap::fromImage(*img));
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

//识别人脸
QImage FaceRec::recognize_face(QImage image)
{
    Mat InputMat = QImage2cvMat(image);
    CascadeClassifier ccf;   //创建分类器对象
    if(!ccf.load(xmlpath))
    {
        QMessageBox::warning(this,
                             "不能加载指定的xml文件",
                             "不能加载指定的xml文件");
        //return ;
    }
    std::vector<Rect> faces;  //创建一个容器保存检测出来的脸
    Mat gray;
    cv::cvtColor(InputMat,gray,CV_BGR2GRAY); //转换成灰度图，因为harr特征从灰度图中提取
    equalizeHist(gray,gray);  //直方图均衡行
    ccf.detectMultiScale(gray, faces, 1.1, 3, 0, Size(10,10),Size(100,100)); //检测人脸
    //画出脸部矩形框
    for(std::vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
    {
        cv::rectangle(InputMat,*iter,Scalar(0,0,255),2,8); //画出脸部矩形
    }
    QImage result = Mat2QImage(InputMat);
    return result;
}


void FaceRec::on_fillLightButton_clicked()
{
    Mat middle = QImage2cvMat(*img);
    QImage test = Mat2QImage(fill_light(middle));
    ui->label_2->setPixmap(QPixmap::fromImage(test));

}
