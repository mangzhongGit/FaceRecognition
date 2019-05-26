#include <mythread.h>

myThread::myThread(QObject *parent):
    QThread (parent)
{
    stopped = false;
    model = EigenFaceRecognizer::create();
}

void myThread::run()
{
    if(flag == 1)
    {
        model_train(); // 训练分类器
        qDebug()<<"model training successfully";
        stopped = true;
    }

    if(flag == 2)
    {
        QFileInfo modleFile("E:/Git/FaceRecognition/MyFacePCAModel.xml");
        if(modleFile.exists()) // 判断模型文件是否存在
        {
            model->read("E:/Git/FaceRecognition/MyFacePCAModel.xml");
            qDebug()<<"model training successfully";
        }
        else{
            QMessageBox::information(NULL, tr("提示"), "请先点击采集图片进行模型训练");
        }
        stopped = true;
    }

    if(flag == 3)
    {
        vector<Mat> faces = recognize_face(frame0,1);
        Mat face = get_face(faces);
        if(!face.empty()){
            int predictPCA;
            double confidence;
            model_detect(face, model, predictPCA, confidence);
            if(predictPCA == 41)
            {
                emit sendsignal();
                flag = 0;
                stopped = true;
            }
        }
    }
    flag = 0;
    stopped = false;
}

void myThread::stop()
{
    stopped = true;
}

void myThread::receiveMat(Mat frame)
{
    flag = 3;
    frame0 = frame;
}

void myThread::receiveFlag(int flag0)
{
    flag = flag0;
}
