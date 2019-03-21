#ifndef FACEREC_H
#define FACEREC_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class QTimer;

namespace Ui {
class FaceRec;
}

class FaceRec : public QWidget
{
    Q_OBJECT

public:
    explicit FaceRec(QWidget *parent = nullptr);
    ~FaceRec();

private slots:
    void on_pushButton_clicked();//获取本地图像

    void open_camera();

private:
    Ui::FaceRec *ui;

    QString filename;

};

#endif // FACEREC_H
