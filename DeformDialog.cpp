#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QMessageBox>
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
    connect(ui->buttonDetectCamera, SIGNAL(clicked(bool)), this, SLOT(onButtonEnableCamera()));
    connect(ui->buttonReset, SIGNAL(clicked(bool)), ui->deformWidget, SLOT(reset()));
    connect(ui->buttonTracking, SIGNAL(toggled(bool)), this, SLOT(onButtonEnableTracking(bool)));
}

DeformDialog::~DeformDialog()
{
    delete ui;
}

void DeformDialog::onButtonLoadImage()
{
    QString metadataFileName = QFileDialog::getOpenFileName(this,
        tr("Open Image Metadata"), QDir::currentPath(), tr("Metadata (metadata.txt)"));
    if(metadataFileName.isEmpty())
    {
        return;
    }
    ui->deformWidget->loadMetadata(metadataFileName);
}

void DeformDialog::onButtonEdgeDetect()
{
    ui->deformWidget->performEdgeDetect();
}

void DeformDialog::onButtonEnableCamera()
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if(cameras.size() == 0)
    {
        QMessageBox::warning(this, "No camera available.", "No camera available.");
        return;
    }
    foreach (const QCameraInfo &cameraInfo, cameras) {
        camera = new QCamera(cameraInfo);
        QMessageBox::information(this, "Camera found.", "Using camera " + cameraInfo.deviceName());
        ui->faceAlignmentWidget->setCamera(camera);
        ui->faceAlignmentWidget->startCapture();
        return;
    }
}

void DeformDialog::onButtonEnableTracking(bool toggle)
{
    if(toggle)
    {
        qDebug() << "onButtonEnableTracking";
        ui->deformWidget->enbaleTracking(
            ui->faceAlignmentWidget->faceXOriginalLandmarks,
            ui->faceAlignmentWidget->faceXLandmarks);
    }
    else
    {
        qDebug() << "onButtonDisableTracking";
        ui->deformWidget->disableTracking();
    }
}
