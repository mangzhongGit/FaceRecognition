/**
    * 这个文件中存放一些工具函数
*/
#include <tools.h>

// cv::Mat类型转换为QImage类型
QImage Mat2QImage(const cv::Mat& InputMat)
{
    if(InputMat.type() == CV_8UC1)
    {
        QImage image(InputMat.cols, InputMat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = InputMat.data;
        for(int row = 0; row < InputMat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, InputMat.cols);
            pSrc += InputMat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(InputMat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)InputMat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, InputMat.cols, InputMat.rows, InputMat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(InputMat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)InputMat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, InputMat.cols, InputMat.rows, InputMat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

//QImage类型转换为cv:Mat类型
cv::Mat QImage2cvMat(const QImage& image)
{
    cv::Mat TmpMat;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        TmpMat = cv::Mat(image.height(), image.width(), CV_8UC4,
                         (void *)image.constBits(),static_cast<size_t>(image.bytesPerLine()));
        break;
    case QImage::Format_RGB888:
        TmpMat = cv::Mat(image.height(), image.width(), CV_8UC3,
                         (void*)image.constBits(), static_cast<size_t>(image.bytesPerLine()));
        cv::cvtColor(TmpMat, TmpMat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        TmpMat = cv::Mat(image.height(), image.width(), CV_8UC1,
                         (void*)image.constBits(), static_cast<size_t>(image.bytesPerLine()));
        break;
    default:
        break;
    }
    return TmpMat.clone();
}

//识别人脸
vector<Mat> recognize_face(Mat InputMat, int flag)
{
    CascadeClassifier ccf;   //创建分类器对象
    std::string xmlpath = "D:\\opencv-3.4.5\\opencv-3.4.5-build\\install\\etc"
                          "\\haarcascades\\haarcascade_frontalface_default.xml";
    if(!ccf.load(xmlpath))
    {
        QMessageBox::warning(NULL,
                             "不能加载指定的xml文件",
                             "不能加载指定的xml文件");
    }
    std::vector<Rect> faces;  //创建一个容器保存检测出来的脸
    Mat gray;
    cv::cvtColor(InputMat,gray,CV_BGR2GRAY); //转换成灰度图，因为harr特征从灰度图中提取
    equalizeHist(gray,gray);  //直方图均衡话
    ccf.detectMultiScale(gray, faces, 1.1, 3, 0); //检测人脸
    vector<Mat> result;
    //画出脸部矩形框
    if(flag == 0)//画出脸部矩形
    {
        for(std::vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
        {
            cv::rectangle(InputMat,*iter,Scalar(0,0,255),2,8); //画出脸部矩形
        }
        result.push_back(InputMat);
    }
    else if(flag == 1){ //截取人脸
        for(size_t i=0;i<faces.size();i++)
        {
            Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

            //将输入的图片中的多个人脸放入result中
            result.push_back(InputMat(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height)));
        }
    }
    return result;
}
Mat get_face(vector<Mat> InputMat)
{
    Mat face;
    for(uint i=0,big=0;i<InputMat.size();i++)
    {
        uint size = InputMat[i].cols * InputMat[i].rows;
        if(big < size)
        {
            big = size;
            face = InputMat[i];
        }
    }
    return face;
}

void model_detect(Mat InputMat, Ptr<EigenFaceRecognizer> model, int &predictPCA, double &confidence)
{
    Mat gray;
    cv::cvtColor(InputMat,gray,CV_BGR2GRAY);
    cv::resize(gray, gray, cv::Size(92, 112));
    model->predict(gray,predictPCA,confidence);
    cout <<  predictPCA << " "<<confidence << endl;
}

// 细节增强

/**
    使用了三个尺度的高斯模糊，再和原图做减法，
    获得不同程度的细节信息，然后通过一定的组合方式把这些细节信息融合到原图中，
    从而得到加强原图信息的能力
*/
Mat promote(Mat Src)
{
    int Radius = 5;
    int rows = Src.rows;
    int cols = Src.cols;
    int cha = Src.channels();
    cv::Mat B1, B2, B3;
    GaussianBlur(Src, B1, Size(Radius, Radius), 1.0, 1.0);//高斯模糊
    GaussianBlur(Src, B2, Size(Radius*2-1, Radius*2-1), 2.0, 2.0);
    GaussianBlur(Src, B3, Size(Radius*4-1, Radius*4-1), 4.0, 4.0);

    double w1 = 0.5;
    double w2 = 0.5;
    double w3 = 0.25;

    cv::Mat dest = cv::Mat::zeros(Src.size(), Src.type());
    for (size_t i = 0; i < rows; i++)
    {
        uchar* src_ptr = Src.ptr<uchar>(i);
        uchar* dest_ptr = dest.ptr<uchar>(i);
        uchar* B1_ptr = B1.ptr<uchar>(i);
        uchar* B2_ptr = B2.ptr<uchar>(i);
        uchar* B3_ptr = B3.ptr<uchar>(i);
        for (size_t j = 0; j < cols; j++)
        {
            for (size_t c = 0; c < cha; c++)
            {
                int  D1 = src_ptr[3*j+c] - B1_ptr[3 * j + c];
                int  D2 = B1_ptr[3 * j + c] - B2_ptr[3 * j + c];
                int  D3 = B2_ptr[3 * j + c] - B3_ptr[3 * j + c];
                int  sign = (D1 > 0) ? 1 : -1;
                dest_ptr[3 * j + c] = saturate_cast<uchar>((1 - w1*sign)*D1 - w2*D2 + w3*D3 + src_ptr[3 * j + c]);
            }
        }
    }
    return dest;
}

void change_image(int flag)
{
    if(flag == 1) // 生成三种不同光照的训练图像
    {
        QDir dir;
        QString path("D:/TEST/s41");
        if(!dir.exists())
            qDebug() << "文件目录不存在";
        else
        {
            dir.cd(path);
            QFileInfoList list = dir.entryInfoList();

            for(int i=2; i<list.count(); i++)
            {
                QFileInfo file = list[i];

                Mat img = imread(file.filePath().toStdString());
                Mat middle,gray;
                String pgmname;
                middle = img.clone();
                middle = auto_fill_light(middle);
                middle = auto_adjust_light(middle);
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                pgmname = format("D:/TEST/s42/%d.pgm", i-1);
                imwrite(pgmname, gray);

                middle = img.clone();
                middle = enhanced_local_texture_feature(middle);
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                pgmname = format("D:\\TEST\\s43\\%d.pgm", i-1);
                imwrite(pgmname, gray);

                middle = img.clone();
                middle = integated_algorithm(middle);
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                pgmname = format("D:\\TEST\\s44\\%d.pgm", i-1);
                imwrite(pgmname, gray);
            }
        }
    }

    if(flag == 2) //将原图像转化为pgm格式
    {
        QDir dir;
        QString path("D:/TEST/t");
        if(!dir.exists())
            qDebug() << "文件目录不存在";
        else
        {
            dir.cd(path);
            QFileInfoList list = dir.entryInfoList();

            for(int i=2; i<list.count(); i++)
            {
                QFileInfo file = list[i];

                Mat img = imread(file.filePath().toStdString());
                Mat middle,gray;
                String pgmname;
                middle = img.clone();
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                cv::resize(gray, gray, cv::Size(92, 112));
                pgmname = format("D:\\TEST\\t1\\%d.pgm", i-1);
                imwrite(pgmname, gray);
            }
        }
    }

    if(flag == 3) //生成测试图像
    {
        QDir dir;
        QString path("D:/TEST/t1");
        if(!dir.exists())
            qDebug() << "文件目录不存在";
        else
        {
            dir.cd(path);
            QFileInfoList list = dir.entryInfoList();

            for(int i=2; i<list.count(); i++)
            {
                QFileInfo file = list[i];

                Mat img = imread(file.filePath().toStdString());
                Mat middle,gray;
                String pgmname;
                middle = img.clone();
                middle = auto_fill_light(middle);
                middle = auto_adjust_light(middle);
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                pgmname = format("D:/TEST/t2/%d.pgm", i-1);
                imwrite(pgmname, gray);

                middle = img.clone();
                middle = enhanced_local_texture_feature(middle);
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                pgmname = format("D:\\TEST\\t3\\%d.pgm", i-1);
                imwrite(pgmname, gray);

                middle = img.clone();
                middle = integated_algorithm(middle);
                cv::cvtColor(middle,gray,CV_BGR2GRAY);
                pgmname = format("D:\\TEST\\t4\\%d.pgm", i-1);
                imwrite(pgmname, gray);
            }
        }
    }
}

void test(Ptr<EigenFaceRecognizer> model)
{
    QDir dir;
    QString path("D:/TEST/t4");
    qDebug()<<"*****图像测试*****";
    if(!dir.exists())
        qDebug() << "文件目录不存在";
    else
    {
        dir.cd(path);
        QFileInfoList list = dir.entryInfoList();

        for(int i=2; i<list.count(); i++)
        {
            QFileInfo file = list[i];
            Mat img = imread(file.filePath().toStdString());
            Mat middle = img.clone();
            int predictPCA;
            double confidence;
            model_detect(middle, model, predictPCA, confidence);
        }
    }

}
