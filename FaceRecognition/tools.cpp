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
    cout << "the predict result is " << predictPCA << endl << "confidence is " << confidence << endl;
}
