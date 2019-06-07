/**
    * 这个文件中存放处理图片光照的函数
*/
#include <tools.h>

// 补光操作
cv::Mat fill_light(Mat InputMat)
{
    // 设置增亮映射表
    int arrayColor[256];
    for(int i=0; i<256; i++)
    {
        arrayColor[i] = i+1;
        if(arrayColor[i]>=255) arrayColor[i]=254;
    }

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

// 自动补光操作
cv::Mat auto_fill_light(Mat InputMat)
{
    Mat result = InputMat;
    int flag = judge_light_intensity(InputMat);
    switch(flag)
    {
    case 1: //光照过强
//      result = fill_light(result);
        break;
    case -1: //光照不足
        result = fill_light(result);
        auto_fill_light(result);
        break;
    default:
        break;
    }
    return result;
}

// 自动调整光照 Local Color Correction Using Non-Linear Masking 算法实现
Mat auto_adjust_light(Mat InputMat)
{
    Mat TmpMat, gray, op_gray, BlurMat, sketch, result = InputMat;
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
    // cv::resize(BlurMat,sketch,Size(BlurMat.cols/2,BlurMat.rows/2),0,0,INTER_LINEAR);
    MatIterator_<Vec3b> colorit, colorend, colorit2, colorend2, colorit3, colorend3;
    colorit = InputMat.begin<Vec3b>();
    colorend = InputMat.end<Vec3b>();
    colorit2 = BlurMat.begin<Vec3b>();
    colorit3 = result.begin<Vec3b>();

    for(; colorit != colorend; colorit++, colorit2++, colorit3++)
    {
        for(int i=0;i<3;i++)
        {
            double middle = ((double)(128 - (*colorit2)[i])) / 128;
            middle = (double)pow(2,middle);
            double middle_2 = ((double)(*colorit)[i]) / 255;
            (*colorit3)[i]= (int)(255 * pow(middle_2, middle));
        }
    }
    return result;
}

/**
    * OpenCV 检测图片亮度
    * 计算图片在灰度图上的均值和方差
    * 当存在亮度异常时，均值会偏离均值点（可以假设为128），方差也会偏小；
    * 通过计算灰度图的均值和方差，就可评估图像是否存在过曝光或曝光不足。
    * 返回 -1 表示亮度过暗
    * 返回 0  表示亮度正常
    * 返回 1  表示亮度过亮
*/
int judge_light_intensity(Mat InputMat)
{
    float demoBright;
    // 获取demo亮度
    demoBright = 0;
    string demoPath = "E:\\Git\\FaceRecognition\\TestCase\\demo.jpg";
    Mat demo = imread(demoPath);
    if(!demo.empty())
    {
        demoBright = get_brightness(demo);
    }
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
    float da = a / (float)(gray.rows * gray.cols);
    float d = fabs(da);
    float ma = 0;
    for(int i=0; i<256; i++)
    {
        ma += fabs(i-128-da)*Hist[i];
    }
    ma = (float)(ma / (gray.rows * gray.cols));
    float m = fabs(ma);
    float flag = d/m;
    if(flag >= 1)
    {
        if(da > demoBright)  return 1;
        else return -1;
    }
    else return 0;
}

// 灰度世界法 实现白平衡
Mat balance_white(Mat InputMat)
{
    Mat result;
    std::vector<cv::Mat> g_vChannels;
    //分离通道
    split(InputMat,g_vChannels);
    Mat imageBlueChannel = g_vChannels.at(0);
    Mat imageGreenChannel = g_vChannels.at(1);
    Mat imageRedChannel = g_vChannels.at(2);

    double imageBlueChannelAvg=0;
    double imageGreenChannelAvg=0;
    double imageRedChannelAvg=0;

    //求各通道的平均值
    imageBlueChannelAvg = mean(imageBlueChannel)[0];
    imageGreenChannelAvg = mean(imageGreenChannel)[0];
    imageRedChannelAvg = mean(imageRedChannel)[0];

    //求出个通道所占增益
    double K = (imageRedChannelAvg+imageGreenChannelAvg+imageRedChannelAvg)/3;
    double Kb = K/imageBlueChannelAvg;
    double Kg = K/imageGreenChannelAvg;
    double Kr = K/imageRedChannelAvg;

    //更新白平衡后的各通道BGR值
    addWeighted(imageBlueChannel,Kb,0,0,0,imageBlueChannel);
    addWeighted(imageGreenChannel,Kg,0,0,0,imageGreenChannel);
    addWeighted(imageRedChannel,Kr,0,0,0,imageRedChannel);

    merge(g_vChannels,result);//图像各通道合并
    imshow("result",result);
    return result;
}

// gamma 校验
Mat gamma_correct(Mat InputMat, float fGamma)
{
    Mat result;
    unsigned char lut[256];
    for(int i=0; i<256; i++)
    {
        lut[i] = (uchar)(pow((float)(i/255.0), fGamma) * 255.0f);
    }

    result = InputMat;
    const int channels = result.channels();
    if(channels == 1)
    {
        MatIterator_<uchar> it, end;
        for( it = result.begin<uchar>(), end = result.end<uchar>(); it != end; it++ )
            //*it = pow((float)(((*it))/255.0), fGamma) * 255.0;
            *it = lut[(*it)];
    }
    if(channels == 3)
    {
        MatIterator_<Vec3b> it, end;
        for( it = result.begin<Vec3b>(), end = result.end<Vec3b>(); it != end; it++ )
        {
            //(*it)[0] = pow((float)(((*it)[0])/255.0), fGamma) * 255.0;
            //(*it)[1] = pow((float)(((*it)[1])/255.0), fGamma) * 255.0;
            //(*it)[2] = pow((float)(((*it)[2])/255.0), fGamma) * 255.0;
            (*it)[0] = lut[((*it)[0])];
            (*it)[1] = lut[((*it)[1])];
            (*it)[2] = lut[((*it)[2])];
        }
    }
    return result;
}
// 增强的局部纹理特征集，在困难的灯光下进行人脸识别
Mat enhanced_local_texture_feature(Mat  InputMat)
{
    Mat result = InputMat, mFace1, mFace2;
    double sigma1 = 1, sigma2 = 2, alpha = 0.1, tau  = 10;
    float fGamma = 1.8;

    // 直接读取灰度图会得到CV_8UC1类型的mat，是单通道uchar型矩阵，
    // 因此高斯滤波后相减都是整型非负数据，影响后面进行比较取极值的步骤。
    // 所以需要将原本的数据类型转化为CV_32FC1即单通道float型数据。再进行后续的操作
    result.convertTo(result,CV_32FC1, 1.0/255);

    // gamma correction
    Mat gammaMat = gamma_correct(result, fGamma);

    // DOG filter
    Mat dst1, dst2;
    int dia = 9;
    GaussianBlur(gammaMat, dst1, Size(dia,dia), sigma1, sigma1);
    GaussianBlur(gammaMat, dst2, Size(dia,dia), sigma2, sigma2);
    result = dst1 - dst2;

    // Contrast Equalization
    mFace1 = cv::abs(result);
    cv::pow(mFace1, alpha, mFace1);
    result = result / cv::pow(cv::mean(mFace1).val[0], 1/alpha); //mean用于求Mat均值

    mFace1 = cv::abs(result);
    mFace1 = (cv::min)(tau, mFace1);
    cv::pow(mFace1, alpha, mFace1);
    result  = result / cv::pow(cv::mean(mFace1).val[0], 1/alpha) / tau;

    cv::exp(result, mFace1); //exp 求每个矩阵元素 src(I) 的自然数 e 的 src(I) 次幂 dst[I] = esrc(I)
    cv::exp((0-result), mFace2);
    result  = mFace1 - mFace2;
    mFace1 = mFace1 + mFace2;
    result  = result / mFace1;

    cv::normalize(result, result, 0, 1, CV_MINMAX);                       //归一化
    cv::convertScaleAbs(result, result, 255);                             //32f->8u

    return result;
}

Mat integated_algorithm(Mat InputMat)
{
    Mat mat0 = auto_fill_light(InputMat);
    Mat mat1 = auto_adjust_light(mat0);
    Mat result = promote(mat1);

//    Mat result = mat1, mFace1, mFace2;
//    double sigma1 = 1, sigma2 = 2, alpha = 0.1, tau  = 10;

//    // 直接读取灰度图会得到CV_8UC1类型的mat，是单通道uchar型矩阵，
//    // 因此高斯滤波后相减都是整型非负数据，影响后面进行比较取极值的步骤。
//    // 所以需要将原本的数据类型转化为CV_32FC1即单通道float型数据。再进行后续的操作
//    result.convertTo(result,CV_32FC1, 1.0/255);
//    float fGamma = 1.8;
//    result = gamma_correct(result, fGamma);

//    // DOG filter
//    Mat dst1, dst2;
//    int dia = 9;
//    GaussianBlur(result, dst1, Size(dia,dia), sigma1, sigma1);
//    GaussianBlur(result, dst2, Size(dia,dia), sigma2, sigma2);
//    result = dst1 - dst2;

//    // Contrast Equalization
//    mFace1 = cv::abs(result);
//    cv::pow(mFace1, alpha, mFace1);
//    result = result / cv::pow(cv::mean(mFace1).val[0], 1/alpha); //mean用于求Mat均值

//    mFace1 = cv::abs(result);
//    mFace1 = (cv::min)(tau, mFace1);
//    cv::pow(mFace1, alpha, mFace1);
//    result  = result / cv::pow(cv::mean(mFace1).val[0], 1/alpha) / tau;

//    cv::exp(result, mFace1); //exp 求每个矩阵元素 src(I) 的自然数 e 的 src(I) 次幂 dst[I] = esrc(I)
//    cv::exp((0-result), mFace2);
//    result  = mFace1 - mFace2;
//    mFace1 = mFace1 + mFace2;
//    result  = result / mFace1;

//    cv::normalize(result, result, 0, 1, CV_MINMAX);                       //归一化
//    cv::convertScaleAbs(result, result, 255);                             //32f->8u

    return result;
}

float get_brightness(Mat InputMat)
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
