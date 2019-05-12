#ifndef TAKEPHOTO_H
#define TAKEPHOTO_H

#include <headers.h>
#include <tools.h>
namespace Ui {
class takePhoto;
}

class takePhoto : public QWidget
{
    Q_OBJECT

public:
    explicit takePhoto(QWidget *parent = nullptr);
    ~takePhoto();

signals:
    void sendsignal();

private slots:
    void on_pushButton_clicked();

private:
    Ui::takePhoto *ui;
    VideoCapture cap; //视频获取结构， 用来作为视频获取函数的一个参数
    void open_camera();     //打开摄像头
    void get_own_photo();
};

#endif // TAKEPHOTO_H
