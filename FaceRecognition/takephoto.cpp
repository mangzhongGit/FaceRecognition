#include "takephoto.h"
#include "ui_takephoto.h"

takePhoto::takePhoto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::takePhoto)
{
    ui->setupUi(this);
    initID = 41;
    // 数据库设置
//    if(!createConnection())
//    {
//        QMessageBox::warning(NULL,tr("waring"),tr("数据库连接失败！"));
//    }
//    else
//    {
    model = new QSqlQueryModel(this);
    model->setQuery("select * from face");
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    model->setHeaderData(2, Qt::Horizontal, tr("性别"));
    model->setHeaderData(3, Qt::Horizontal, tr("年龄"));
    model->setHeaderData(4, Qt::Horizontal, tr("注册时间"));

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    }

    ui->label_5->setAlignment(Qt::AlignCenter);
    folder = new QDir;
}

takePhoto::~takePhoto()
{
    delete ui;
}

void takePhoto::get_own_photo(QString filepath)
{
    cap.open(0);
    if(!cap.isOpened())
    {
        QMessageBox::warning(this,
                             tr("frame is empty"),
                             tr("frame is empty"));
        exit(-1);
    }
    int flag = 1, i = 1;
    while(flag)
    {
        char key = waitKey(100);
        String winName = "Press P to take a photo, press q to quit";
        if(i==21)
        {
            QMessageBox::information(NULL, "result", "头像采集完毕！");
            // 重新采集头像，就需要重新进行模型训练
            cap.release();
            destroyWindow(winName);
            break;
        }
        Mat frame;
        cap>>frame;
//        std::string text = "请做出不同的表情，按下P采集您的照片";
//        int font_face = cv::FONT_HERSHEY_COMPLEX;
//        double font_scale = 1;
//        int thickness = 1;
//        int baseline;
//        //获取文本框的长宽
//        cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
//        cv::Point origin;
//        origin.x = frame.cols / 2 - text_size.width / 2;
//        origin.y = frame.rows / 2 + text_size.height / 2;
//        cv::putText(frame, text, origin, font_face, font_scale, cv::Scalar(255, 0, 0), thickness, 8, 0);

        String pgmname = format("\\%d.pgm", i);
        pgmname = filepath.toStdString() + pgmname;
        Mat gray;
        vector<Mat> faceMat;
        Mat face;
        imshow(winName,frame);
        switch(key)
        {
            case 'p':
                faceMat = recognize_face(frame,1);
                if(faceMat.size() == 0){QMessageBox::warning(NULL,tr("警告"),tr("未检测到人脸"));}
                else{
                    for(uint j=0,big=0;j<faceMat.size();j++)
                    {
                        uint size = faceMat[j].cols * faceMat[j].rows;
                        if(big < size)
                        {
                            big = size;
                            face = faceMat[j];
                        }
                    }
                    i++;
                    face = auto_fill_light(face);
                    face = auto_adjust_light(face);
//                    face = integated_algorithm(face);
                    cv::cvtColor(face,gray,CV_BGR2GRAY);
                    cv::resize(gray, gray, cv::Size(92, 112));
                    imwrite(pgmname, gray);
                    imshow("photo", face);
                    waitKey(1000);
                    destroyWindow("photo");
                }
                break;
            case 'q':
                cap.release();
                destroyWindow(winName);
                flag=0;
                break;
            default:
                break;
        }
    }
}

void takePhoto::on_pushButton_clicked()
{
    int intID = initID + model->rowCount();
    QString id = QString::number(intID);
    QString filepath = "E:\\Git\\FaceRecognition\\att_faces\\";
    filepath = filepath + "s" + id;
    bool exist = folder->exists(filepath);
    if(exist)
    {
        QMessageBox::warning(this,tr("创建文件夹"),tr("文件夹已经存在！"));
    }else
        {
        //创建新文件夹
        folder->mkdir(filepath);
    }

    add_new_data();
    get_own_photo(filepath);
}

void takePhoto::add_new_data()
{
    // ID
    int intID = initID + model->rowCount();
    QString id = QString::number(intID);
    // 姓名
    QString name = ui->lineEdit->text();
    // 性别
    QString gender = ui->lineEdit_2->text();
    // 年龄
    QString age = ui->lineEdit_3->text();
    // 日期
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString dateTimeStr = curDateTime.toString("MM-dd HH:mm:ss");
    QString data = "insert into face values(";
    data = data + id + ",";
    data = data + "'" + name + "'" + ",";
    data = data + "'" + gender + "'" + ",";
    data = data + age + ",";
    data = data + "'" + dateTimeStr + "'" + ")";
    qDebug()<<data;
    model->setQuery(data);

}

void takePhoto::on_refreshDBButton_clicked()
{
    model->setQuery("select * from face");
}
//删除选中行
void takePhoto::on_deleteLineButton_clicked()
{
    int curRow = ui->tableView->currentIndex().row();
    int id = model->index(curRow,0).data().toInt();

    // 删除存放人脸图像的文件夹
    QString filepath = "E:\\Git\\FaceRecognition\\att_faces\\";
    filepath = filepath + "s" + QString::number(id);
    QDir dir(filepath);
    if(!dir.exists())
    {
        QMessageBox::warning(this,tr("删除文件夹"),tr("文件夹不存在！"));
    }
    else
    {
        dir.cd(filepath);
        QFileInfoList list = dir.entryInfoList();
        // 删除文件夹中的所有文件
        for(int i=2; i<list.count(); i++)
        {
            QFileInfo file = list[i];
            file.dir().remove(file.fileName());
        }
        // 删除文件夹
        dir.rmdir(filepath);
    }

    model->setQuery(QString("delete from face where id=%1").arg(id));
    model->setQuery("select * from face");
}

void takePhoto::on_returnButton_clicked()
{
    emit sendsignals();
    //this->close();
}
