#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "facerec.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


// 页面跳转槽函数
void MainWindow::on_pushButton_clicked()
{
    FaceRec *f = new FaceRec();
    QFont font("ZYSong18030",12);//设置字体
    f->setFont(font);
    f->setWindowTitle(tr("人脸识别系统"));
    this->close();
    f->show();
}
