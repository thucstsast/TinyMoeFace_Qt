#include <vector>
#include <opencv/cv.hpp>
#include <QDebug>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include "DeformWidget.h"
#include "FaceOutlineReader.h"

DeformWidget::DeformWidget(QWidget *parent) : QWidget(parent)
{
}

bool DeformWidget::loadMetadata(const QString &path)
{
    QString outlineFileName;
    QStringList layersFileName;
    QDir dataDir = QFileInfo(path).dir();
    QFile metadataFile(path);
    if(!metadataFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream metadataFileStream(&metadataFile);
    outlineFileName = dataDir.absoluteFilePath(metadataFileStream.readLine());
    while(!metadataFileStream.atEnd())
    {
        QString line = metadataFileStream.readLine();
        QStringList parts = line.split(' ');
        QString layerName = parts[0];
        QString layerFileName = dataDir.absoluteFilePath(parts[1]);
        auto image = new QImage(layerFileName);
        layers[layerName] = image;
        if(imageWidth < 0)
        {
            imageWidth = image->width();
            imageHeight = image->height();
        }
        else
        {
            Q_ASSERT(imageWidth == image->width());
            Q_ASSERT(imageHeight == image->height());
        }
        layerOrder.append(layerName);
        layersFileName.append(layerFileName);
    }
    metadataFile.close();
    outlineReader.openFile(outlineFileName);
    outlineReader.parseSvgOutline(imageWidth, imageHeight);
}

void DeformWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    for(const QString& layerName : layerOrder)
    {
        painter.drawImage(0, 0, *layers[layerName]);
    }
    const auto &outline = outlineReader.getOutlines();
    QPainterPath path;
    QMapIterator<QString, QVector<QPointF>> i(outline);
    while (i.hasNext())
    {
        i.next();
        for(const QPointF& point : i.value())
        {
            qDebug() << point;
            painter.setBrush(QBrush(Qt::red));
            painter.drawEllipse(point, 3, 3);
        }
    }
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
    cv::RNG rng(12345);
    QImage alphaMask(image->width(), image->height(), QImage::Format_Grayscale8);
    for(int i = 0; i < alphaMask.width(); i++)
    {
        for(int j = 0; j < alphaMask.height(); j++)
        {
            uint8_t alpha = qAlpha(image->pixel(i, j));
            alphaMask.setPixel(i, j, qRgb(alpha, alpha, alpha));
        }
    }
    *image = alphaMask;
    //QImage rgbImage = image->convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(image->height(),image->width(),CV_8U,alphaMask.scanLine(0));
    cv::Mat matG(image->height(),image->width(),CV_8U);
    cv::Mat mat2(image->height(),image->width(),CV_8U);
    //cv::GaussianBlur(mat, matG, cv::Size(11, 11), 10);
    cv::Canny(mat, mat2, 100, 200, 3);
    std::vector<std::vector<cv::Point> > shapeContours;
    std::vector<cv::Vec4i> contoursHierarchy;
    //cv::GeneralizedHoughBallard gh();
    //gh.detect(mat, mat2);
    cv::findContours(mat2, shapeContours, contoursHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    cv::Mat drawing = cv::Mat::zeros( mat2.size(), CV_8UC3 );
    for(int i = 0; i < shapeContours.size(); i++)
    {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        cv::drawContours( drawing, shapeContours, i, color, 2, 8, contoursHierarchy, 0, cv::Point() );
    }
    cv::namedWindow("Display window", 100);
    cv::imshow("Display window", drawing);
    qDebug() << "Curves found: " << shapeContours.size();
    qDebug() << contoursHierarchy.size();
    for(int i = 0; i < contoursHierarchy.size(); i++)
    {
        cv::Vec4i & temp = contoursHierarchy[i];
        qDebug() << temp[0] << temp[1] << temp[2] << temp[3];
    }
}
