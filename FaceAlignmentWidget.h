#ifndef FACEALIGNMENTWIDGET_H
#define FACEALIGNMENTWIDGET_H

#include <opencv/cv.hpp>
#include <vector>
#include <QCamera>
#include <QWidget>
#include "SimpleCameraSurface.h"

class QCamera;
class FaceX;

class FaceAlignmentWidget : public QWidget
{
    Q_OBJECT
protected:
    QImage currentFrame;
    QCamera *camera;
    SimpleCameraSurface *simpleCameraSurface;
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    cv::CascadeClassifier *openCVcascadeClassifier;
    std::vector<cv::Point2d> faceXLandmarks;
    FaceX *faceXObject;
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
