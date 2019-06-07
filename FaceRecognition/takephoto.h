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
    void sendsignals();

private slots:
    void on_pushButton_clicked();

    void on_refreshDBButton_clicked();

    void on_deleteLineButton_clicked();

    void on_returnButton_clicked();

private:
    Ui::takePhoto *ui;
    VideoCapture cap; //视频获取结构， 用来作为视频获取函数的一个参数
    void get_own_photo(QString filepath);

    QSqlQueryModel *model;
    int initID;
    QSqlTableModel *tableModel;
    QDir *folder;

    void add_new_data();
};

#endif // TAKEPHOTO_H
