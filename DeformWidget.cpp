#include "CGALTriangulator.h"
#include <vector>
#include <opencv/cv.hpp>

uint qHash(const Vertex_handle &key)
{
    std::cout <<"111\n";
    return (intptr_t)((void*)(key.operator->()));
    std::cout <<"222\n";
    //return ret;
}

#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include "CGALUtil.h"
#include "DeformWidget.h"
#include "FaceOutlineReader.h"
#include <QDebug>

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
    const auto &outlines = outlineReader.getOutlines();
    QMapIterator<QString, QVector<QPointF>> it(outlines);
    qDebug() << "@Line 52";
    while (it.hasNext())
    {
        it.next();
        qDebug() << "@Line 56";
        const QVector<QPointF>& points = it.value();
        QVector<Vertex_handle> vertexList;
        for(int i = 0; i < points.size(); i++)
        {
            Vertex_handle vh = cdt.insert(Point(points[i].x(), points[i].y()));
            vertexList.append(vh);
            cgalOutlinesSet.insert(vh);
        }
        cgalOutlines.insert(it.key(), vertexList);
        qDebug() << "@Line 63";
        for(int i = 0; i < vertexList.size() - 1; i++)
        {
            //cdt.insert(Point(2, 0.6));
            cdt.insert_constraint(vertexList[i], vertexList[i + 1]);
        }
        cdt.insert_constraint(vertexList.last(), vertexList[0]);
    }
    qDebug() << "@Line 68";
    /*Vertex_handle va = cdt.insert(Point(-4,0));
    Vertex_handle vb = cdt.insert(Point(0,-1));
    Vertex_handle vc = cdt.insert(Point(4,0));
    Vertex_handle vd = cdt.insert(Point(0,1));
    cdt.insert(Point(2, 0.6));
    cdt.insert_constraint(va, vb);
    cdt.insert_constraint(vb, vc);
    cdt.insert_constraint(vc, vd);
    cdt.insert_constraint(vd, va);*/
    CGALTriangulator triangulator;
    triangulator.triangulate(cdt);
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
            painter.drawEllipse(point, 2, 2);
        }
    }
    qDebug() << cdt.number_of_vertices();
    for(auto i = cdt.faces_begin(); i != cdt.faces_end(); i++)
    {
        CDT::Face &face = *i;
        QPointF point0 = CGALUtil::toQtPointF(face.vertex(0)->point());
        QPointF point1 = CGALUtil::toQtPointF(face.vertex(1)->point());
        QPointF point2 = CGALUtil::toQtPointF(face.vertex(2)->point());
        painter.drawLine(point0, point1);
        painter.drawLine(point1, point2);
        painter.drawLine(point2, point0);
    }
    for(auto i = cdt.vertices_begin(); i != cdt.vertices_end(); i++)
    {
        Vertex_handle p = i;
        if(cgalOutlinesSet.contains(p))
        {
            painter.setBrush(QBrush(Qt::red));
        }
        else
        {
            painter.setBrush(QBrush(Qt::yellow));
        }
        painter.drawEllipse(CGALUtil::toQtPointF(p->point()), 2, 2);
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
