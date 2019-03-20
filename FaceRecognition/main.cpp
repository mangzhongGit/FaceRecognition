#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFont f("ZYSong18030",12);

    w.setFont(f);
    w.setWindowTitle("");
    w.show();

    return a.exec();
}
