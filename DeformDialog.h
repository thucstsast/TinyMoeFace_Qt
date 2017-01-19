#ifndef DEFORMDIALOG_H
#define DEFORMDIALOG_H

#include <QDialog>

namespace Ui {
class DeformDialog;
}

class DeformDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeformDialog(QWidget *parent = 0);
    ~DeformDialog();

public slots:
    void onButtonLoadImage();

private:
    Ui::DeformDialog *ui;
};

#endif // DEFORMDIALOG_H
