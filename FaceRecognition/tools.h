#ifndef TOOLS_H
#define TOOLS_H
#include <headers.h>

QImage Mat2QImage(const cv::Mat& InputMat);   //cv::Mat类型转换为QImage类型

cv::Mat QImage2cvMat(const QImage &Image);    //QImage类型转换为cv:Mat类型

int similarity_compare(Mat Mat1, Mat Mat2);

void model_train();

vector<Mat> recognize_face(Mat InputMat, int flag);     //识别人脸

Mat get_face(vector<Mat> InputMat);

void model_detect(Mat InputMat,Ptr<EigenFaceRecognizer> model, int &predictPCA, double &confidence);

cv::Mat fill_light(Mat InputMat);//补光

cv::Mat auto_fill_light(Mat InputMat);

Mat auto_adjust_light(Mat InputMat);

int judge_light_intensity(Mat InputMat);

Mat balance_white(Mat InputMat);

Mat gamma_correct(Mat InputMat, float fGamma);

Mat enhanced_local_texture_feature(Mat  InputMat);

Mat integated_algorithm(Mat InputMat);

float get_brightness(Mat InputMat);

Mat promote(Mat Src);

void change_image(int flag);

void test(Ptr<EigenFaceRecognizer> model);

bool createConnection(); //创建数据库

#endif // TOOLS_H
