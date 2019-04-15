/*
 *  这个文件中存放一些工具函数
*/
#include <facerec.h>

// cv::Mat类型转换为QImage类型
QImage FaceRec::Mat2QImage(const cv::Mat& InputMat)
{
    cv::Mat TmpMat;
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
cv::Mat FaceRec::fill_light(Mat inputMat)
{
    Mat result = inputMat;
    double blue = 0, green = 0, red = 0;
    int count = inputMat.rows * inputMat.cols;
    // 使用迭代器获取Mat图像的RGB值
    MatIterator_<Vec3b> colorit, colorend;
    for(colorit = inputMat.begin<Vec3b>(),colorend = inputMat.end<Vec3b>(); colorit != colorend; ++colorit)
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
    colorit2 = inputMat.begin<Vec3b>();
    colorend2 = inputMat.end<Vec3b>();
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
