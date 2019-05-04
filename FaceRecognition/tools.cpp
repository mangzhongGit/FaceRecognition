/*
 *  这个文件中存放一些工具函数
*/
#include <facerec.h>

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
    ccf.detectMultiScale(gray, faces, 1.1, 3, 0); //检测人脸
    //画出脸部矩形框
    for(std::vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
    {
        cv::rectangle(InputMat,*iter,Scalar(0,0,255),2,8); //画出脸部矩形
    }
    QImage result = Mat2QImage(InputMat);
    return result;
}

// cv::Mat类型转换为QImage类型
QImage FaceRec::Mat2QImage(const cv::Mat& InputMat)
{
    cv::Mat TmpMat, gray;
    //将颜色空间转换为RGB
    if (InputMat.channels() == 1)
    {
        cv::cvtColor(InputMat, TmpMat, CV_GRAY2RGB);
    }
    else
    {
        cv::cvtColor(InputMat, TmpMat, CV_BGR2RGB);
    }
    //使用mat的数据构造qimage，而不复制数据
    QImage Result = QImage(static_cast<const uchar*>(TmpMat.data),
                           TmpMat.cols,TmpMat.rows, QImage::Format_RGB888);
    //深拷贝数据从Mat到QImage
    Result.bits();
    return Result;
}

//QImage类型转换为cv:Mat类型
cv::Mat FaceRec::QImage2cvMat(const QImage& image)
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

// 补光操作
cv::Mat FaceRec::fill_light(Mat InputMat)
{
    Mat result = InputMat;
    double blue = 0, green = 0, red = 0;
    int count = InputMat.rows * InputMat.cols;
    // 使用迭代器获取Mat图像的RGB值
    MatIterator_<Vec3b> colorit, colorend;
    for(colorit = InputMat.begin<Vec3b>(),colorend = InputMat.end<Vec3b>(); colorit != colorend; ++colorit)
    {
        blue += (*colorit)[0];
        green += (*colorit)[1];
        red += (*colorit)[2];
    }
    // 获得三个通道各自的颜色均值
    blue = blue / count;
    green = green / count;
    red = red / count;

    // 获得均值灰度值
    int gray = static_cast<int>(0.299*red + 0.587*green + 0.114*blue);
    // 获得第一映射颜色值
    int colorMap1 =arrayColor[gray];
    // 获得滤色灰度值
    int colorScreen = (255- ((255-gray)*(255-colorMap1)) / 255);
    // 获得透明度值
    int versionGray = 150; // 这是一个预设值，在128-200之间即可
    int Alpha = std::min(1, std::max(0, (versionGray - gray)/(colorMap1 - gray)));

    MatIterator_<Vec3b> colorit2, colorend2, colorit3, colorend3;
    colorit2 = InputMat.begin<Vec3b>();
    colorend2 = InputMat.end<Vec3b>();
    colorit3 = result.begin<Vec3b>();
    colorend3 = result.end<Vec3b>();
    for( ;colorit2 != colorend2; ++colorit2, ++colorit3)
    {
        for(int i=0; i<3; i++)
        {
            int color = (*colorit2)[i];
            int colorMap2 = arrayColor[color];
            int colorScreen2 = (255 - ((255-color)*(255-colorMap2)/255));
            (*colorit3)[i] = static_cast<int>(Alpha*colorScreen2 + (1.0-Alpha)*color);
        }
    }

    return result;
}

// 自动调整光照 Local Color Correction Using Non-Linear Masking 算法实现
Mat FaceRec::auto_adjust_light(Mat InputMat)
{
    Mat TmpMat, gray, op_gray, BlurMat, sketch, result;
    // 将颜色空间转换为RGB
    if (InputMat.channels() == 1)
    {
        cv::cvtColor(InputMat, TmpMat, CV_GRAY2RGB);
    }
    else
    {
        cv::cvtColor(InputMat, TmpMat, CV_BGR2RGB);
    }

    // 将RGB图像转化为灰度图像
    cv::cvtColor(TmpMat,gray,CV_BGR2GRAY);

    // 将灰度图像取反
    op_gray = 255 - gray;

    // 用大半径滤波器将取反的灰度图像模糊
    GaussianBlur(op_gray, BlurMat, Size(15,15), 0);

    // 利用缩略图来获取图像的mask值
    cv::resize(BlurMat,sketch,Size(BlurMat.cols/2,BlurMat.rows/2),0,0,INTER_LINEAR);

    imshow("InputMat", InputMat);
    imshow("TmpMat", TmpMat);
    imshow("gray", gray);
    imshow("op_gray", op_gray);
    imshow("BlurMat", BlurMat);
    imshow("sketch", sketch);

    result = 255 * (InputMat / 255);
    return BlurMat;
}
