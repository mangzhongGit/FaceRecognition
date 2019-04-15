#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFont font("ZYSong18030",12);

    w.setFont(font);
    w.setWindowTitle("");
    w.show();

    return a.exec();
}
