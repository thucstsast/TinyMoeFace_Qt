#ifndef DEFORMDIALOG_H
#define DEFORMDIALOG_H

#include <QDialog>

namespace Ui {
class DeformDialog;
}

class QCamera;

class DeformDialog : public QDialog
{
    Q_OBJECT

protected:
    QCamera *camera;

public:
    explicit DeformDialog(QWidget *parent = 0);
    ~DeformDialog();

public slots:
    void onButtonLoadImage();
    void onButtonEdgeDetect();
    void onButtonEnableCamera();
    void onButtonEnableTracking(bool toggle);

private:
    Ui::DeformDialog *ui;
};

#endif // DEFORMDIALOG_H
