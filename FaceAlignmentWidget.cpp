#include <QCamera>
#include <QPainter>
#include "face_x.h"
#include "FaceAlignmentWidget.h"

FaceAlignmentWidget::FaceAlignmentWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(100, 100);
    simpleCameraSurface = new SimpleCameraSurface(this);
    connect(simpleCameraSurface, SIGNAL(frameAvailable(QImage)), this, SLOT(onFrameAvailable(QImage)));
    faceXObject = new FaceX("model.xml.gz");
    faceXLandmarks.resize(faceXObject->landmarks_count());
    openCVcascadeClassifier = new cv::CascadeClassifier("haarcascade_frontalface_alt2.xml");
}

void FaceAlignmentWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, currentFrame);
    for (cv::Point2d p : faceXLandmarks)
    {
        painter.drawEllipse(p.x, p.y, 4, 4);
    }

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
    //cout << "Press \"r\" to re-initialize the face location." << endl;
    QImage rgbImage = currentFrame.convertToFormat(QImage::Format_RGB888);
    cv::Mat cvFrame(rgbImage.height(),rgbImage.width(),CV_8UC3,rgbImage.scanLine(0));
    cv::Mat img;

        cv::cvtColor(cvFrame, img, cv::COLOR_BGR2GRAY);
        //cv::imshow("Gray image", img);

        std::vector<cv::Point2d> original_landmarks = faceXLandmarks;
        faceXLandmarks = faceXObject->Alignment(img, faceXLandmarks);

        for (int i = 0; i < faceXLandmarks.size(); ++i)
        {
            faceXLandmarks[i].x = (faceXLandmarks[i].x + original_landmarks[i].x) / 2;
            faceXLandmarks[i].y = (faceXLandmarks[i].y + original_landmarks[i].y) / 2;
        }

        /*for (cv::Point2d p : landmarks)
        {
            cv::circle(frame, p, 1, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("\"r\" to re-initialize, \"q\" to exit", frame);
        int key = cv::waitKey(10);
        if (key == 'q')
            break;
        else if (key == 'r')
        {

        }*/

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

void FaceAlignmentWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    std::vector<cv::Rect> faces;
    QImage rgbImage = currentFrame.convertToFormat(QImage::Format_RGB888);
    cv::Mat cvFrame(rgbImage.height(),rgbImage.width(),CV_8UC3,rgbImage.scanLine(0));
    cv::Mat img;
    cv::cvtColor(cvFrame, img, cv::COLOR_BGR2GRAY);
    openCVcascadeClassifier->detectMultiScale(img, faces);
    if (!faces.empty())
    {
        faceXLandmarks = faceXObject->Alignment(img, faces[0]);
    }
}
