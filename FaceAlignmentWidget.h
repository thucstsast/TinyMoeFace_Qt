#ifndef FACEALIGNMENTWIDGET_H
#define FACEALIGNMENTWIDGET_H

#include <QCamera>
#include <QWidget>
#include "SimpleCameraSurface.h"

class QCamera;

class FaceAlignmentWidget : public QWidget
{
    Q_OBJECT
protected:
    QImage currentFrame;
    QCamera *camera;
    SimpleCameraSurface *simpleCameraSurface;
    void paintEvent(QPaintEvent * event) override;
public:
    explicit FaceAlignmentWidget(QWidget *parent = 0);
    void setCamera(QCamera *camera);
    void startCapture();
    void stopCapture();

signals:

public slots:
    void onFrameAvailable(QImage frame);
    void onCameraStateChanged(QCamera::State state);
};

#endif // FACEALIGNMENTWIDGET_H
