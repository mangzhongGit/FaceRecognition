#ifndef FACEREC_H
#define FACEREC_H

#include <QWidget>

namespace Ui {
class FaceRec;
}

class FaceRec : public QWidget
{
    Q_OBJECT

public:
    explicit FaceRec(QWidget *parent = nullptr);
    ~FaceRec();

private:
    Ui::FaceRec *ui;
};

#endif // FACEREC_H
