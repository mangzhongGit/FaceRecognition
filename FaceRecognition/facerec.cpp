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
    model = EigenFaceRecognizer::create();
    judgeMe = 0;
    thread = new myThread();

    // 获取demo亮度
    demoBright = 0;
    string demoPath = "E:\\Git\\FaceRecognition\\TestCase\\demo.jpg";
    Mat demo = imread(demoPath);
    if(!demo.empty())
    {
        demoBright = get_brightness(demo);
    }
    // 设置增亮映射表
    for(int i=0; i<256; i++)
    {
        arrayColor[i] = i+1;
        if(arrayColor[i]>=255) arrayColor[i]=254;
    }

    // 设置tab页名称
    ui->tabWidget->setTabText(0,"算法效果测试");
    ui->tabWidget->setTabText(1,"人脸识别系统");
    // 设置显示图片label的大小
    ui->label->setFixedSize(400, 400);
    ui->label->setText("原图");
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label_2->setFixedSize(400,400);
    ui->label_2->setText("效果图");
    ui->label_2->setAlignment(Qt::AlignCenter);
    ui->label_3->setFixedSize(400, 200);
    ui->label_4->setFixedSize(400, 200);

    // 信号和槽
    connect(timer, SIGNAL(timeout()),this,SLOT(read_frame()));
    connect(this, SIGNAL(sendMat(Mat)), thread, SLOT(receiveMat(Mat)));
    connect(this, SIGNAL(sendFlag(int)), thread, SLOT(receiveFlag(int)));
    connect(thread,SIGNAL(sendsignal()), this, SLOT(receiceSignal()));
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
                                            tr("Images (*.png *.bmp *.jpg *.tif *.GIF *.pgm)"));
    qDebug()<<filename;
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
        // 判断图片的大小，如果图片过大，需要进行压缩处理，避免出现内存问题
        int width = (*img).width(), height = (*img).height();
        while(width >1000 || height > 1000)
        {
            (*img) = (*img).scaled(width/2, height/2);
            width = (*img).width();
            height = (*img).height();
        }
        imgMat = cv::imread(filename.toStdString());

        // 让图片自适应label的大小
        QPixmap *pixmap = new QPixmap(QPixmap::fromImage(*img));
        pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
        ui->label->setScaledContents(true);
        ui->label->setPixmap(*pixmap);

        QString text = get_information(imgMat);
        ui->label_3->setText(text);
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
    //imshow("FaceRecgnitizing",frame);
    QImage imgMiddle = Mat2QImage(frame);
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(imgMiddle));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->videoLabel->setScaledContents(true);
    ui->videoLabel->setPixmap(*pixmap);
    thread->start();
    emit sendMat(frame);
}

void FaceRec::receiceSignal()
{
    qDebug()<<"识别成功，你是张吉栋";
    thread->stop();
    on_closeCameraButton_clicked();
    //destroyWindow("FaceRecgnitizing");
}

//拍照
void FaceRec::on_takePhotoButton_clicked()
{
    get_chart();
//    cap>>frame;
//    *img = Mat2QImage(frame);
//    ui->label_2->setPixmap(QPixmap::fromImage(*img));
}

//关闭摄像头
void FaceRec::on_closeCameraButton_clicked()
{
    timer->stop();
    cap.release();
}

void FaceRec::on_takeOwnPhotoButton_clicked()
{
    QDir dir;
    QString path("E:/Git/FaceRecognition/att_faces/s41");
    if(!dir.exists())
        qDebug() << "文件目录不存在";
    else
    {
        dir.cd(path);
        QFileInfoList list = dir.entryInfoList();
        if(list.count()<=2)//需要采集自己的照片
        {
            takePhoto *photo = new takePhoto();
            connect(photo,SIGNAL(sendsignal()),this,SLOT(reshow())); //用于从子界面返回到主界面
            QFont font("ZYSong18030",12);//设置字体
            photo->setFont(font);
            photo->setWindowTitle(tr("人脸识别系统"));
            photo->show();
            this->hide();
        }
        else
        {
            QMessageBox message(QMessageBox::NoIcon, tr("提示"), tr("您的照片已采集，不需要再次采集照片！如需重新采集，请点击Yes"),
                                QMessageBox::Yes | QMessageBox::No, NULL);
            if(message.exec() == QMessageBox::Yes)
            {
                for(int i=2; i<list.count(); i++)
                {
                    QFileInfo file = list[i];
                    file.dir().remove(file.fileName());
                }
                on_takeOwnPhotoButton_clicked();
            }
        }
    }
}

void FaceRec::reshow()
{
    this->show();
}

void FaceRec::get_chart()
{
//    QPieSeries *series = new QPieSeries();
//    series->append("相似度70%", 7);
//    series->append("30%", 3);
//    series->setLabelsVisible();

//    QPieSlice *slice_red = series->slices().at(0);
//    QPieSlice *slice_green = series->slices().at(1);

//    slice_red->setColor(QColor(255,0,0,255));
//    slice_green->setColor(QColor(0,255,0,255));

//    QChart *chart = new QChart();
//    chart->addSeries(series);
//    chart->setTitle("PieChart Example");
//    chart->legend()->hide();
//    ui->graphicsView->setChart(chart);
//    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

float FaceRec::get_brightness(Mat InputMat)
{
    Mat gray;
    // 将RGB图像转化为灰度图像
    cv::cvtColor(InputMat,gray,CV_BGR2GRAY);
    float a=0;
    int Hist[256] = {0};
    for(int i=0; i<gray.rows; i++)
    {
        for(int j=0; j<gray.cols; j++)
        {
            a += (float)(gray.at<uchar>(i,j) - 128);
            int x = (int)gray.at<uchar>(i,j);
            Hist[x]++;
        }
    }
    float brightness = a / (float)(gray.rows * gray.cols);
    return brightness;
}

QString FaceRec::get_information(Mat InputMat)
{
    Mat middleMat = InputMat.clone();
    QString s_text = "原图信息显示：";
    QString s_type = "图像类型：";

    if(middleMat.channels() == 1)
        s_type = s_type + "灰度图";
    else
        s_type = s_type + "RGB图";
    QString s_size = "图像大小：" + QString::number(middleMat.cols) +"*" + QString::number(middleMat.rows)+ " piexl";
    QString s_brightness = "图像亮度：" + QString("%1").arg(get_brightness(middleMat));
    int s_flag = judge_light_intensity(middleMat);
    QString s_judge = "图像判定：";
    switch(s_flag)
    {
        case 1: //光照过强
            s_judge = s_judge + "光照过强，图像光亮";
            break;
        case -1: //光照不足
            s_judge = s_judge + "光照不足，图像过暗";
            break;
        default:
            s_judge = s_judge + "光照正常";
            break;
    }
    s_text = s_text + "\n" + s_type + "\n" + s_size + "\n" + s_brightness + "\n" + s_judge;
    return s_text;
}
// 调用自动补光算法
void FaceRec::on_fillBrightButton_clicked()
{
    Mat middle = imgMat.clone();
    Mat result = auto_fill_light(middle);
    QImage imgMiddle = Mat2QImage(result);
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(imgMiddle));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(*pixmap);

    QString text = get_information(result);
    ui->label_4->setText(text);
}
// 调用非线性调节光照算法
void FaceRec::on_nonLineButton_clicked()
{
    Mat middle = imgMat.clone();
    Mat result = auto_adjust_light(middle);
    QImage imgMiddle = Mat2QImage(result);
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(imgMiddle));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(*pixmap);

    QString text = get_information(result);
    ui->label_4->setText(text);
}
// 调用人脸识别局部纹理特征集增强算法
void FaceRec::on_faceEnButton_clicked()
{
    Mat middle = imgMat.clone();
    Mat result = enhanced_local_texture_feature(middle);
    QImage imgMiddle = Mat2QImage(result);
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(imgMiddle));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(*pixmap);

    QString text = get_information(result);
    ui->label_4->setText(text);
}

void FaceRec::on_modelTrainButton_clicked()
{
    thread->start();
    emit sendFlag(1);
}

void FaceRec::on_loadXMLButton_clicked()
{
    thread->start();
    emit sendFlag(2);
}
