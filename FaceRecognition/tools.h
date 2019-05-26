#ifndef TOOLS_H
#define TOOLS_H
#include <headers.h>

// XML文件即是我们人脸检测所需要的分类器文件

QImage Mat2QImage(const cv::Mat& InputMat);   //cv::Mat类型转换为QImage类型

cv::Mat QImage2cvMat(const QImage &Image);    //QImage类型转换为cv:Mat类型

int similarity_compare(Mat Mat1, Mat Mat2);

void model_train();

vector<Mat> recognize_face(Mat InputMat, int flag);     //识别人脸

Mat get_face(vector<Mat> InputMat);

void model_detect(Mat InputMat,Ptr<EigenFaceRecognizer> model, int &predictPCA, double &confidence);

#endif // TOOLS_H
