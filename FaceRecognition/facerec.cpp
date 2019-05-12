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
    QFileInfo modleFile("E:/Git/FaceRecognition/MyFacePCAModel.xml");
    if(modleFile.exists()) // 判断模型文件是否存在
    {
        qDebug()<<"xml file read success";
        model->read("E:/Git/FaceRecognition/MyFacePCAModel.xml");
    }
    else{
        QMessageBox::information(NULL, tr("提示"), "请先点击采集图片进行模型训练");
    }

    for(int i=0; i<256; i++)
    {
        arrayColor[i] = i+1;
        if(arrayColor[i]>=255) arrayColor[i]=254;
    }

    // 设置显示图片label的大小
    ui->label->setFixedSize(200, 200);
    ui->label->setText("原图");
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label_2->setFixedSize(200, 200);
    ui->label_2->setText("效果图");
    ui->label_2->setAlignment(Qt::AlignCenter);
    ui->label_3->setFixedSize(200, 200);
    ui->label_3->setText("测试图");
    ui->label_3->setAlignment(Qt::AlignCenter);
    ui->label_4->setFixedSize(200, 200);
    ui->label_4->setText("原图");
    ui->label_4->setAlignment(Qt::AlignCenter);
    ui->label_5->setFixedSize(200, 200);
    ui->label_5->setText("效果图");
    ui->label_5->setAlignment(Qt::AlignCenter);
    ui->label_6->setFixedSize(200, 200);
    ui->label_6->setText("效果图");
    ui->label_6->setAlignment(Qt::AlignCenter);

    // 信号和槽
    connect(timer, SIGNAL(timeout()),this,SLOT(read_frame()));
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
        // 判断图片的大小，如果图片过大，需要进行压缩处理，避免出现内存问题
        int width = (*img).width(), height = (*img).height();
        while(width >1000 || height > 1000)
        {
            (*img) = (*img).scaled(width/2, height/2);
            width = (*img).width();
            height = (*img).height();
        }
        imgMat = QImage2cvMat(*img);

        // 让图片自适应label的大小
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
    ui->label->setPixmap(QPixmap::fromImage(*img));  //将图片显示到label上
}

//拍照
void FaceRec::on_takePhotoButton_clicked()
{
    get_chart();
    demoMat=balance_white(demoMat);
    testMat=balance_white(testMat);
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

// 补光按钮操作
void FaceRec::on_fillLightButton_clicked()
{
    qDebug()<<"************************************";
    qDebug()<<"dome bright"<<get_brightness(demoMat);
    qDebug()<<"test bright"<<get_brightness(testMat);

    vector<Mat> mat1 = recognize_face(demoMat,1);
    vector<Mat> mat2 = recognize_face(testMat,1);
    Mat face1,face2;
    qDebug() << mat1.size() << mat2.size();
    if(mat1.size() == 0)
    {
        ui->label_3->setText("没有识别到人脸");
    }
    else{
        face1 = get_face(mat1);
        if(face1.empty())
        {
            ui->label_3->setText("没有识别到人脸");
        }
        else{
            QImage qimg = Mat2QImage(face1);
            // 图片自适应 label
            QPixmap *pixmap = new QPixmap(QPixmap::fromImage(qimg));
            pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
            ui->label_3->setScaledContents(true);
            ui->label_3->setPixmap(*pixmap);
            model_detect(face1);
        }
    }

    if(mat2.size() == 0)
    {
        ui->label_6->setText("没有识别到人脸");
    }else{
        face2 = get_face(mat2);
        if(face2.empty())
        {
            ui->label_6->setText("没有识别到人脸");
        }else{
            QImage qimg = Mat2QImage(face2);
            QPixmap *pixmap = new QPixmap(QPixmap::fromImage(qimg));
            pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
            ui->label_6->setScaledContents(true);
            ui->label_6->setPixmap(*pixmap);
            model_detect(face2);
        }
    }

//    if(mat1.size() != 0 && mat2.size() != 0)
//    {
//        qDebug() << "sim  " << similarity_compare(face1,face2);
//    }
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
    QPieSeries *series = new QPieSeries();
    series->append("相似度70%", 7);
    series->append("30%", 3);
    series->setLabelsVisible();

    QPieSlice *slice_red = series->slices().at(0);
    QPieSlice *slice_green = series->slices().at(1);

    slice_red->setColor(QColor(255,0,0,255));
    slice_green->setColor(QColor(0,255,0,255));

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("PieChart Example");
    chart->legend()->hide();
//    QChartView *chartview = new QChartView(chart);
//    chartview->setRenderHint(QPainter::Antialiasing);
//    chartview->show();
    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

void FaceRec::model_detect(Mat InputMat)
{
    Mat gray;
    cv::cvtColor(InputMat,gray,CV_BGR2GRAY);
    cv::resize(gray, gray, cv::Size(92, 112));
    double confidence;
    int predictPCA;
    model->predict(gray,predictPCA,confidence);
    cout << "the predict result is " << predictPCA << endl << "confidence is " << confidence << endl;
}


void FaceRec::on_demoButton_clicked()
{
    QString path = ui->demoLine->text();
    if(path.isEmpty())
    {
        ui->demoLine->setText(tr("请输入地址！"));
    }
    else
    {
        std::string str = path.toStdString();
        demoMat = cv::imread(str);
        QImage qimg = Mat2QImage(demoMat);
        // 图片自适应 label
        QPixmap *pixmap = new QPixmap(QPixmap::fromImage(qimg));
        pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
        ui->label->setScaledContents(true);
        ui->label->setPixmap(*pixmap);
    }
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

void FaceRec::on_testButton_clicked()
{
    QString path = ui->testLine->text();
    if(path.isEmpty())
    {
        ui->testLine->setText(tr("请输入地址！"));
    }
    else
    {
        std::string str = path.toStdString();
        testMat = cv::imread(str);
        QImage qimg = Mat2QImage(testMat);
        // 图片自适应 label
        QPixmap *pixmap = new QPixmap(QPixmap::fromImage(qimg));
        pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
        ui->label_4->setScaledContents(true);
        ui->label_4->setPixmap(*pixmap);
    }
}

void FaceRec::on_pushButton_clicked()
{
    demoMat = auto_adjust_light(demoMat);
    QImage qimg = Mat2QImage(demoMat);
    // 图片自适应 label
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(qimg));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(*pixmap);
}

void FaceRec::on_pushButton_2_clicked()
{
    testMat = auto_fill_light(testMat);
    QImage qimg = Mat2QImage(testMat);
    // 图片自适应 label
    QPixmap *pixmap = new QPixmap(QPixmap::fromImage(qimg));
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label_5->setScaledContents(true);
    ui->label_5->setPixmap(*pixmap);
}
