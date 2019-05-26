#include "takephoto.h"
#include "ui_takephoto.h"

takePhoto::takePhoto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::takePhoto)
{
    ui->setupUi(this);
}

takePhoto::~takePhoto()
{
    delete ui;
}

void takePhoto::open_camera()
{
    cap = NULL;
    cap.open(0);
    if(!cap.isOpened())
    {
        QMessageBox::warning(this,
                             tr("frame is empty"),
                             tr("frame is empty"));
    }
}

void takePhoto::get_own_photo()
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
        if(i==20)
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

        String pgmname = format("E:\\Git\\FaceRecognition\\att_faces\\s41\\%d.pgm", i);
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
    // 发送信号返回主界面
    emit sendsignal();
    this->close();
}

void takePhoto::on_pushButton_clicked()
{
    get_own_photo();
}
