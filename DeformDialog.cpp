#include <QDebug>
#include <QFileDialog>
#include "DeformDialog.h"
#include "ui_DeformDialog.h"

DeformDialog::DeformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeformDialog)
{
    ui->setupUi(this);
    connect(ui->buttonLoadImage, SIGNAL(clicked(bool)), this, SLOT(onButtonLoadImage()));
    connect(ui->buttonEdgeDetect, SIGNAL(clicked(bool)), this, SLOT(onButtonEdgeDetect()));
}

DeformDialog::~DeformDialog()
{
    delete ui;
}

void DeformDialog::onButtonLoadImage()
{
    QString imageFileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if(imageFileName.isEmpty())
    {
        return;
    }
    ui->deformWidget->loadImage(imageFileName);
}

void DeformDialog::onButtonEdgeDetect()
{
    ui->deformWidget->performEdgeDetect();
}
