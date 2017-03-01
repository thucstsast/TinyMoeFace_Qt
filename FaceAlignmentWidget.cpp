#include <QCamera>
#include <QPainter>
#include "FaceAlignmentWidget.h"

FaceAlignmentWidget::FaceAlignmentWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(100, 100);
    simpleCameraSurface = new SimpleCameraSurface(this);
    connect(simpleCameraSurface, SIGNAL(frameAvailable(QImage)), this, SLOT(onFrameAvailable(QImage)));
}

void FaceAlignmentWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, currentFrame);
}

void FaceAlignmentWidget::setCamera(QCamera *camera)
{
    this->camera = camera;
    this->camera->setViewfinder(simpleCameraSurface);
    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(onCameraStateChanged(QCamera::State)));
}

void FaceAlignmentWidget::startCapture()
{
    this->camera->load();
    this->camera->start();
}

void FaceAlignmentWidget::stopCapture()
{
}

void FaceAlignmentWidget::onFrameAvailable(QImage frame)
{
    qDebug() << frame.size();
    currentFrame = frame;
    repaint();
}

void FaceAlignmentWidget::onCameraStateChanged(QCamera::State state)
{
    if(state == QCamera::UnloadedState)
    {
        return;
    }
    //Choose a reasonable resolution
    auto viewFinderSettings = camera->viewfinderSettings();
    viewFinderSettings.setResolution(QSize(320, 240));
    camera->setViewfinderSettings(viewFinderSettings);
    setMinimumSize(320, 240);
}
