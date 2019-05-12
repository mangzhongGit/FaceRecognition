/**
  * 此文件写了人脸识别中的模型训练函数
  *
*/
#include <headers.h>

void get_csv_file()
{
    QFile csvFile("E:/Git/FaceRecognition/at.csv");
    csvFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QDir dir,dir2;
    QString path("E:/Git/FaceRecognition/att_faces");
    if(!dir.exists())
        qDebug() << "文件目录不存在";
    else
    {
        dir.cd(path);
        QStringList list = dir.entryList();
        for(int i = 2; i < list.count(); i++)
        {
            QString dirName = list[i];
            QString flag = dirName.mid(1);
            QString path2 = "E:/Git/FaceRecognition/att_faces/" + dirName;
            dir2.cd(path2);
            QStringList list2 = dir2.entryList();
            for(int j=2;j<list2.count();j++)
            {
                QFileInfo info(list2[j]);
                QString fileName = info.fileName();
                QString content = path + "/" + dirName + "/" + fileName + ";" + flag + "\n";
                csvFile.write(content.toUtf8());
            }
        }
    }
    csvFile.close();
}

int read_csv(QString &filename, std::vector<cv::Mat> &images, std::vector<int> &labels)
{
    char separator = ';';
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
            QByteArray line = file.readLine();
            QString strLine(line);
            QStringList lineList = strLine.split(separator);
            std::string imagePath = lineList.at(0).toStdString();
            int label = lineList.at(1).toInt();
            images.push_back(cv::imread(imagePath,0));
            labels.push_back(label);
        }
        return 1; //读取文件成功
    }
    else
    {
        return 0; //读取文件失败
    }
}

void model_train()
{
    get_csv_file();
    QString csvPath = "E:/Git/FaceRecognition/at.csv";

    // 2个容器来存放图像数据和对应的标签
    std::vector<cv::Mat> images;
    std::vector<int> labels;

    // 读取csv文件
    if(read_csv(csvPath, images, labels) == 0)
    {
        qDebug() << "文件读取失败！";
        return;
    }
    else if(images.size() <= 1)
    {
        qDebug() << "照片数量不够，模型训练最少应该有2张照片！";
    }
    else{
        Ptr<EigenFaceRecognizer> model = EigenFaceRecognizer::create();
        model->train(images, labels);
        model->save("E:/Git/FaceRecognition/MyFacePCAModel.xml");
        qDebug() << "train OK";
    }
}
