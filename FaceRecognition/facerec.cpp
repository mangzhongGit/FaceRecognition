#include "facerec.h"
#include "ui_facerec.h"
#include "mythread.h"

FaceRec::FaceRec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaceRec)
{
    ui->setupUi(this);
    qRegisterMetaType<Mat>("Mat");
    //初始化
    timer = new QTimer(this);
    recTimer = new QTimer(this);
    img = new QImage();
    model = EigenFaceRecognizer::create();

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

    ui->videoLabel->setFixedSize(550, 550);
    ui->videoLabel->setText("视频");
    ui->videoLabel->setAlignment(Qt::AlignCenter);

    ui->faceLabel->setFixedSize(250, 250);
    ui->faceLabel->setText("face");
    ui->faceLabel->setAlignment(Qt::AlignCenter);

    ui->label_5->setFixedSize(250, 350);
    ui->label_5->setText("result");

    // 连接数据库
    if(!createConnection())
    {
        QMessageBox::warning(NULL,tr("waring"),tr("数据库连接失败！"));
    }
    else
    {
        query = new QSqlQuery();
    }


    // 信号和槽
    connect(timer, SIGNAL(timeout()),this,SLOT(read_frame()));
    connect(recTimer, SIGNAL(timeout()),this,SLOT(stopRec()));
    thread = new myThread();
    connect(this, SIGNAL(sendFlag(int)), thread, SLOT(receiveFlag(int)));
    connect(thread, SIGNAL(sendFlag(int)), this, SLOT(receiveFlag(int)));
    connect(thread,SIGNAL(sendsignal(int, Mat, double)), this, SLOT(receiceSignal(int, Mat, double)));
    connect(this, SIGNAL(sendMat(Mat,int)), thread, SLOT(receiveMat(Mat,int)));
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
        cv::resize(imgMat, imgMat, cv::Size(width, height));
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
    if(cap.isOpened()) cap.release();
    cap.open(0);
    if(!cap.isOpened())
    {
        QMessageBox::warning(this,
                             tr("frame is empty"),
                             tr("frame is empty"));
    }
    timer->start(50);   //开始计时，不可缺少，控制帧率，在这里为25帧
    recTimer->start(10000);
    startTime = QTime::currentTime();
    judgeMe = 0;
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
    emit sendMat(frame,judgeMe);
    thread->start();
}

void FaceRec::receiceSignal(int predictPCA, Mat face, double confidence)
{
    closeCamera();
    thread->stop();
    recTimer->stop();
    if(judgeMe == 0)
    {
        judgeMe = 1;
        stopTime = QTime::currentTime();
        int elapsed = startTime.msecsTo(stopTime); //时间差单位为 毫秒
        QString result = "识别成功！！！\n\n" ;
        QString name, gender;
        int age = 0;
        query->exec("select * from face");
        while(query->next())
        {
            if(predictPCA == query->value(0).toInt())
            {
                name = query->value(1).toString();
                gender = query->value(2).toString();
                age = query->value(3).toInt();
            }
        }
        result = result + "姓名："+ name + "\n\n";
        result = result + "性别：" + gender + "\n\n";
        result = result + "年龄：" + QString::number(age) +"岁\n\n";
        result = result + "置信度："+QString::number(confidence,10,4) + "\n\n";
        result = result + "识别时间:" + QString::number(elapsed) + "毫秒";

        QImage imgMiddle = Mat2QImage(face);
        QPixmap *pixmap = new QPixmap(QPixmap::fromImage(imgMiddle));
        pixmap->scaled(ui->faceLabel->size(), Qt::KeepAspectRatio);
        ui->faceLabel->setScaledContents(true);
        ui->faceLabel->setPixmap(*pixmap);

        ui->label_5->setText(result);
    }
}

// 停止识别，已经超时
void FaceRec::stopRec()
{
    recTimer->stop();
    closeCamera();
    ui->label_5->setText(tr("识别失败！！！\n\n识别超时！！！"));
}

//关闭摄像头
void FaceRec::closeCamera()
{
    timer->stop();
    cap.release();
}

void FaceRec::on_takeOwnPhotoButton_clicked()
{
    photo = new takePhoto();
    connect(photo,SIGNAL(sendsignals()),this,SLOT(reshow())); //用于从子界面返回到主界面
    QFont font("ZYSong18030",12);//设置字体
    photo->setFont(font);
    photo->setWindowTitle(tr("人脸注册模块"));
    photo->show();
    this->hide();
}

void FaceRec::reshow()
{
    delete photo;
    this->show();
}

QString FaceRec::get_information(Mat InputMat)
{
    Mat middleMat = InputMat.clone();
    QString s_text = "图像信息显示：";
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

void FaceRec::on_gammaButton_clicked()
{
    QString gammaStr = ui->gammaEdit->text();
    float gamma = gammaStr.toFloat();

    Mat middle = imgMat.clone();
    Mat result = gamma_correct(middle, gamma);
    QImage imgMiddle = Mat2QImage(result);
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(imgMiddle));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(*pixmap);

    QString text = get_information(result);
    ui->label_4->setText(text);
}

void FaceRec::on_improveButton_clicked()
{
    Mat middle = imgMat.clone();
    Mat result = integated_algorithm(middle);
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

void FaceRec::receiveFlag(int flag)
{
    QString text;
    switch(flag){
    case 1:
        text = "分类器训练成功！";
        break;
    case 2:
        text = "分类器器加载成功！";
        break;
    default:
        break;
    }
    thread->stop();
    ui->label_5->setText(text);
}

void FaceRec::on_pushButton_clicked()
{
//    change_image(1);
//    change_image(3);
    thread->start();
    emit sendFlag(4);
}
