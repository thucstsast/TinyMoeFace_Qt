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
    if(currentFrame.size().width() < 10)
        return;
    QPainter painter(this);
    painter.drawImage(0, 0, currentFrame);

    //cout << "Press \"r\" to re-initialize the face location." << endl;
    QImage rgbImage = currentFrame.convertToFormat(QImage::Format_RGB888);
    cv::Mat cvFrame(rgbImage.height(),rgbImage.width(),CV_8UC3,rgbImage.scanLine(0));
    cv::Mat img;

        cv::cvtColor(cvFrame, img, cv::COLOR_BGR2GRAY);
        //cv::Mat denoiseFrame;
        //cv::fastNlMeansDenoisingColored(cvFrame, denoiseFrame);
        //cv::imshow("Gray image", img);

        std::vector<cv::Point2d> original_landmarks = faceXLandmarks;
        //std::vector<cv::Rect> faces;
        //openCVcascadeClassifier->detectMultiScale(img, faces);
        //faceXLandmarks = faceXObject->Alignment(img, faces[0]);
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
    for(int i = 0; i < faceXLandmarks.size(); i++)
    {
        cv::Point2d p = faceXLandmarks[i];
        painter.setPen(Qt::NoPen);
        if(i >= LANDMARK_EYEBROW_LEFT_BEGIN && i <= LANDMARK_EYEBROW_RIGHT_END)
        {
            painter.setBrush(Qt::red);
        }
        else if(i >= LANDMARK_NOSE_BEGIN && i <= LANDMARK_NOSE_END)
        {
            painter.setBrush(Qt::yellow);
        }
        else if(i >= LANDMARK_EYE_LEFT_BEGIN && i <= LANDMARK_EYE_RIGHT_END)
        {
            painter.setBrush(Qt::blue);
        }
        else if(i >= LANDMARK_EYE_MOUSE_BEGIN && i <= LANDMARK_EYE_MOUSE_END)
        {
            painter.setBrush(Qt::black);
        }
        painter.drawEllipse(p.x, p.y, 4, 4);
    }
    for (cv::Point2d p : faceXLandmarks)
    {
        //painter.drawPoint(p.x, p.y);

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
    viewFinderSettings.setResolution(QSize(640, 480));
    camera->setViewfinderSettings(viewFinderSettings);
    qDebug() << "???";
    qDebug() << camera->imageProcessing()->isAvailable();
    camera->imageProcessing()->setDenoisingLevel(1.0);
    setMinimumSize(640, 480);
}

void FaceAlignmentWidget::mousePressEvent(QMouseEvent *event)
{
    //QWidget::mousePressEvent(event);
    std::vector<cv::Rect> faces;
    QImage rgbImage = currentFrame.convertToFormat(QImage::Format_RGB888);
    cv::Mat cvFrame(rgbImage.height(),rgbImage.width(),CV_8UC3,rgbImage.scanLine(0));
    //cv::Mat denoiseFrame;
    //cv::fastNlMeansDenoisingColored(cvFrame, denoiseFrame);
    cv::Mat img;
    cv::cvtColor(cvFrame, img, cv::COLOR_BGR2GRAY);
    openCVcascadeClassifier->detectMultiScale(img, faces);
    if (!faces.empty())
    {
        faceXLandmarks = faceXObject->Alignment(img, faces[0]);
        faceXOriginalLandmarks = faceXLandmarks;
    }
}
