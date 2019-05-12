/**
  * 基于OpenCV与均值哈希算法的人脸相似识别算法
  * 1.缩小尺寸：去除高频和细节的最快方法是缩小图片，将图片缩小到8x8的尺寸，共64个像素。
  * 不用保持纵横比，只需将其变成8x8的正方形，这样就可以摒弃不同尺寸、比例带来的图片差异
  * 2.简化色彩：将8x8的小图片转换成灰度图像
  * 3.计算平均值：计算所有64个像素的灰度平均值
  * 4.比较像素的灰度：将每个像素的灰度，与平均值进行比较，大于或等于就记为1，小于均值，就记为0；
  * 5.计算hash值，将上面的比价比较结果组合在一起，构成了64位的哈希值，就是这张图片的指纹。
  * 6.通过比较两张图片的哈希值，计算差值得到汉明距离。汉明距离越小，说明两张图片越相似

*/

#include <tools.h>

std::string hash_value(Mat src)
{
    std::string result(64,'\0');
    Mat img;
    if(src.channels() == 3)
    {
        cv::cvtColor(src,img,CV_BGR2GRAY);
    }
    else
    {
        img=src.clone();
    }
    cv::resize(img, img, Size(8,8));
    uchar *pData;
    //获取像素值
    for(int i=0; i<img.rows; i++)
    {
        pData=img.ptr<uchar>(i);
        for(int j=0;j<img.cols;j++)
        {
            pData[j]=pData[j]/4;
        }
    }
    // 均值计算
    int average=mean(img).val[0];
    // 比较像素的灰度值
    //将每个像素的值与平均值比较，大于或等于平均值的标记为1，小于的标记为0，作为图像掩码
    Mat mask=(img>=(uchar)average);
    int index=0;

    //计算hash值
    for(int i=0;i<mask.rows;i++)
    {
        pData = mask.ptr<uchar>(i);
        for(int j=0;j<mask.cols;j++)
        {
            if(pData[j]==0)
                result[index++]='0';
            else
                result[index++]='1';
        }
    }
    return result;
}

//汉明距离计算：如果小于5怎可以认为两个图像时相似的
int hanming_dist(std::string &str1, std::string &str2)
{
    if((str1.size() != 64) || (str2.size() != 64))
    {
        return -1;
    }
    int distValue = 0;
    for (int i = 0; i < 64; i++)
    {
        if (str1[i] != str2[i])
        {
            distValue++;
        }
    }
    return distValue;
}

int similarity_compare(Mat Mat1, Mat Mat2)
{
    std::string str1, str2;
    str1 = hash_value(Mat1);
    str2 = hash_value(Mat2);

    int distValue = hanming_dist(str1, str2);
    return distValue;
}
