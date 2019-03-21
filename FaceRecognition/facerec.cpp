#include "facerec.h"
#include "ui_facerec.h"

FaceRec::FaceRec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaceRec)
{
    ui->setupUi(this);
    // 实时采集摄像头图像，设置计时器
    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(open_camera()));

}

FaceRec::~FaceRec()
{
    delete ui;
}
/*
    采集摄像头函数
*/
void FaceRec::open_camera()
{

}
//获取本地图像
void FaceRec::on_pushButton_clicked()
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
        QImage *img = new QImage();
        if(! (img->load(filename))) //加载图像
        {
            QMessageBox::information(this,
                                     tr("图像打开失败"),
                                     tr("图像打开失败"));
            delete img;
            return;
        }
        ui->label->setPixmap(QPixmap::fromImage(*img));
    }
}
