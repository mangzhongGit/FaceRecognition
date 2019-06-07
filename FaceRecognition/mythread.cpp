#include <mythread.h>
myThread::myThread(QObject *parent):
    QThread (parent)
{
    stopped = false;
    model = EigenFaceRecognizer::create();
    qRegisterMetaType<Mat>("Mat");
}

void myThread::run()
{
    qDebug()<<"flag:"<<flag;
    switch(flag){
    case 1:
        model_train(); // 训练分类器
        qDebug()<<"model training successfully";
        emit sendFlag(1);
        flag=0;
        stopped = true;
        break;
    case 2:
    {
        QFileInfo modleFile("E:/Git/FaceRecognition/MyFacePCAModel.xml");
        if(modleFile.exists()) // 判断模型文件是否存在
        {
            model->read("E:/Git/FaceRecognition/MyFacePCAModel.xml");
            qDebug()<<"model load successfully";
            emit sendFlag(2);
        }
        else{
            QMessageBox::information(NULL, tr("提示"), "请先点击采集图片进行模型训练");
        }
        flag=0;
        stopped = true;
        break;
    }
    case 3:
    {
        if(!frameMat.empty() && flag0 == 0)
        {
            uint num = frameMat.size();
            qDebug()<<"num:"<<num;
            Mat frame = frameMat[num-1];
            frameMat.pop_back();
//            Mat frame = frame0;
            vector<Mat> faces = recognize_face(frame,1);
            Mat face = get_face(faces);
            if(!face.empty()){
                qDebug()<<1;
                Mat face1 = face.clone();
                face1 = auto_fill_light(face1);
                face1 = auto_adjust_light(face1);
    //            face1 = integated_algorithm(face1);
                cv::cvtColor(face1,face1,CV_BGR2GRAY);
                cv::resize(face1, face1, cv::Size(92, 112));
                imwrite("E:\\Git\\FaceRecognition\\middle.pgm", face1);
                Mat middle = imread("E:\\Git\\FaceRecognition\\middle.pgm");
                qDebug()<<12;
                face1 = middle;
                int predictPCA;
                double confidence;
                model_detect(face1, model, predictPCA, confidence);
                if(predictPCA >= 41 && confidence<3000)
                {
                    flag0 = 1;
                    emit sendsignal(predictPCA, face, confidence);
                    qDebug()<<123;
                    flag = 0;
                    stopped = true;
                    if(frameMat.size()>0)
                        frameMat.clear();
                }
            }
        }
        break;
    }
    case 4:
        test(model);
        flag=0;
        stopped = true;
        break;
    default:
        flag = 0;
        stopped = false;
        break;
    }
}

void myThread::stop()
{
    stopped = true;
}

void myThread::receiveMat(Mat frame, int judgeMe)
{
    flag0 = judgeMe;
    flag = 3;
    frame0 = frame;
    if(frameMat.size() >= 100)
        frameMat.clear();
    frameMat.push_back(frame);
}

void myThread::receiveFlag(int flag0)
{
    flag = flag0;
}
