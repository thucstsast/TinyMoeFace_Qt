#include <opencv/cv.hpp>
#include <QDebug>
#include <QPainter>
#include "DeformWidget.h"

DeformWidget::DeformWidget(QWidget *parent) : QWidget(parent)
{
}

void DeformWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    if(image == nullptr)
    {
        return;
    }
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
}

bool DeformWidget::loadImage(const QString& path)
{
    if(image != nullptr)
    {
        return false;
    }
    image = new QImage(path);
    if(image->isNull())
    {
        delete image;
        image = nullptr;
        return false;
    }
    setFixedWidth(image->width());
    setFixedHeight(image->height());
    return true;
}

void DeformWidget::performEdgeDetect()
{
    //QImage rgbImage = image->convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(image->height(),image->width(),CV_8UC4,image->scanLine(0));
    cv::Mat mat2(image->height(),image->width(),CV_8UC4);
    cv::Canny(mat, mat2,100,200);
    cv::namedWindow("Display window", 100);
    cv::imshow("Display window", mat2);
    qDebug() << "DISPLAY IMAGE";
}
