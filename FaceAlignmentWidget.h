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
    const static int LANDMARK_EYEBROW_LEFT_BEGIN = 0;
    const static int LANDMARK_EYEBROW_LEFT_END = 4;
    const static int LANDMARK_EYEBROW_RIGHT_BEGIN = 5;
    const static int LANDMARK_EYEBROW_RIGHT_END = 9;
    const static int LANDMARK_NOSE_BEGIN = 10;
    const static int LANDMARK_NOSE_END = 18;
    const static int LANDMARK_EYE_LEFT_BEGIN = 19;
    const static int LANDMARK_EYE_LEFT_END = 24;
    const static int LANDMARK_EYE_RIGHT_BEGIN = 25;
    const static int LANDMARK_EYE_RIGHT_END = 30;
    const static int LANDMARK_EYE_MOUSE_BEGIN = 31;
    const static int LANDMARK_EYE_MOUSE_END = 50;
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
