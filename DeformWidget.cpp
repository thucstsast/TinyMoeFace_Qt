#include "CGALTriangulator.h"
#include "FaceAlignmentWidget.h"
#include <vector>
#include <opencv/cv.hpp>
#include <QColorDialog>

uint qHash(const Vertex_handle &key)
{
    return (intptr_t)((void*)(key.operator->()));
}

#include <QTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include "CGALUtil.h"
#include "DeformWidget.h"
#include "FaceOutlineReader.h"
#include "SimpleDeformer.h"
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
        qDebug() << it.key();
        const QVector<QPointF>& points = it.value();
        qDebug() << points.size();
        QVector<Vertex_handle> vertexList;
        for(int i = 0; i < points.size(); i++)
        {
            Vertex_handle vh = cdt.insert(Point(points[i].x(), points[i].y()));
            vertexList.append(vh);
            cgalOutlinesSet.insert(vh);
            cgalOutlineMaps[it.key()].insert(vh);
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
    deformer = new SimpleDeformer(cdt);
    for(Vertex_handle vertexHandle : cgalOutlinesSet)
    {
        deformer->addConstrainedVertex(vertexHandle);
    }
}

void DeformWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    if(layers.empty())
    {
        return;
    }
    for(const QString& layerName : layerOrder)
    {
        qDebug() << layerName;
        if(layerName != "face" && layerName != "eyebrow" && layerName != "mouth")
        {
            painter.drawImage(0, 0, *layers[layerName]);
        }
        else
        {
            painter.setBrush(QBrush(*layers[layerName]));
            for(auto i = cdt.faces_begin(); i != cdt.faces_end(); i++)
            {
                CDT::Face &face = *i;
                QPointF p1 = CGALUtil::toQtPointF(face.vertex(0)->point());
                QPointF p2 = CGALUtil::toQtPointF(face.vertex(1)->point());
                QPointF p3 = CGALUtil::toQtPointF(face.vertex(2)->point());
                QPointF q1 = deformer->getVertexPosition(face.vertex(0));
                QPointF q2 = deformer->getVertexPosition(face.vertex(1));
                QPointF q3 = deformer->getVertexPosition(face.vertex(2));
                QPolygonF poly1;
                poly1.append(p1);
                poly1.append(p2);
                poly1.append(p3);
                QPolygonF quad1, quad2;
                quad1 << p1 << p2 << (p2 + p3) - p1 << p3;
                quad2 << q1 << q2 << (q2 + q3) - q1 << q3;
                QTransform transform;
                QTransform::quadToQuad(quad1, quad2, transform);
                painter.setTransform(transform);
                painter.drawPolygon(poly1);
            }
            painter.resetTransform();
        }
    }
    /*QPointF q1(10, 110), q2(30, 170), q3(70, 130);
    QPolygonF poly1;
    poly1.append(p1);
    poly1.append(p2);
    poly1.append(p3);
    painter.drawPolygon(poly1);
    painter.drawRect(200, 200, 200, 200);
;*/
    //painter.drawPixmap(0, 0, testPixmap);
    //End testing.


    const auto &outline = outlineReader.getOutlines();
    QPainterPath path;
    QMapIterator<QString, QVector<QPointF>> i(outline);
    for(auto i = cdt.faces_begin(); i != cdt.faces_end(); i++)
    {
        CDT::Face &face = *i;
        QPointF point0 = deformer->getVertexPosition(face.vertex(0));
        QPointF point1 = deformer->getVertexPosition(face.vertex(1));
        QPointF point2 = deformer->getVertexPosition(face.vertex(2));
        painter.drawLine(point0, point1);
        painter.drawLine(point1, point2);
        painter.drawLine(point2, point0);
    }
    for(auto i = cdt.vertices_begin(); i != cdt.vertices_end(); i++)
    {
        Vertex_handle p = i;
        if(hasSelectedVertex && p == selectedVertex)
        {
            painter.setBrush(QBrush(Qt::red));
        }
        else if(cgalOutlinesSet.contains(p))
        {
            painter.setBrush(QBrush(Qt::blue));
        }
        else
        {
            painter.setBrush(QBrush(Qt::yellow));
        }
        painter.drawEllipse(deformer->getVertexPosition(p), 2, 2);
    }
}

void DeformWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!mousePressed) return;
    if(!hasSelectedVertex) return;
    deformer->moveVertex(selectedVertex, QPointF(event->pos()));
    repaint();
}

void DeformWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::RightButton)
    {
        if(event->y() <= 300)
        {
            changeColor("hair");
            qDebug() << "Region1";
        }
        else if(event->y() <= 500)
        {
            changeColor("iris");
            qDebug() << "Region2";
        }
    }
    if(event->button() != Qt::MouseButton::LeftButton)
        return;
    mousePressed = true;
    Vertex_handle nearestVertex;
    bool found = false;
    for(Vertex_handle vertexHandle : cgalOutlinesSet)
    {
        QPointF vertexLocation = deformer->getVertexPosition(vertexHandle);
        auto delta = vertexLocation - QPointF(event->pos());
        if(delta.x() * delta.x() + delta.y() * delta.y() < 5 * 5)
        {
            found = true;
            nearestVertex = vertexHandle;
        }
    }
    if(found)
    {
        selectedVertex = nearestVertex;
        hasSelectedVertex = true;
    }
    else
    {
        hasSelectedVertex = false;
    }
    repaint();
}

void DeformWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() != Qt::MouseButton::LeftButton)
        return;
    mousePressed = false;
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

void DeformWidget::reset()
{
    for(Vertex_handle vertexHandle : cgalOutlinesSet)
    {
        QPointF originalPosition = CGALUtil::toQtPointF(vertexHandle->point());
        deformer->moveVertex(vertexHandle, originalPosition, false);
        /*QPointF vertexLocation = deformer->getVertexPosition(vertexHandle);
        auto delta = vertexLocation - QPointF(event->pos());
        if(delta.x() * delta.x() + delta.y() * delta.y() < 5 * 5)
        {
            found = true;
            nearestVertex = vertexHandle;
        }*/
    }
    deformer->updateVertices();
    repaint();
}

void DeformWidget::evaluateKeyPoints(
        const std::vector<cv::Point2d>* points,
        QPointF& p1, QPointF& p2, QPointF& p3, QPointF& p4, QPointF& p5, QPointF& p6)
{
    p1 = QPointF(0, 0);
    for(int i = FaceAlignmentWidget::LANDMARK_EYE_LEFT_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYE_LEFT_END; i++)
    {
        p1.setX(p1.x() + (*points)[i].x);
        p1.setY(p1.y() + (*points)[i].y);
    }
    p1 /= (FaceAlignmentWidget::LANDMARK_EYE_LEFT_END - FaceAlignmentWidget::LANDMARK_EYE_LEFT_BEGIN + 1);

    p2 = QPointF(0, 0);
    for(int i = FaceAlignmentWidget::LANDMARK_EYE_RIGHT_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYE_RIGHT_END; i++)
    {
        p2.setX(p2.x() + (*points)[i].x);
        p2.setY(p2.y() + (*points)[i].y);
    }
    p2 /= (FaceAlignmentWidget::LANDMARK_EYE_RIGHT_END - FaceAlignmentWidget::LANDMARK_EYE_RIGHT_BEGIN + 1);

    p3 = QPointF(0, 0);
    for(int i = FaceAlignmentWidget::LANDMARK_EYEBROW_LEFT_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYEBROW_LEFT_END; i++)
    {
        p3.setX(p3.x() + (*points)[i].x);
        p3.setY(p3.y() + (*points)[i].y);
    }
    p3 /= (FaceAlignmentWidget::LANDMARK_EYEBROW_LEFT_END - FaceAlignmentWidget::LANDMARK_EYEBROW_LEFT_BEGIN + 1);

    p4 = QPointF(0, 0);
    for(int i = FaceAlignmentWidget::LANDMARK_EYEBROW_RIGHT_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYEBROW_RIGHT_END; i++)
    {
        p4.setX(p4.x() + (*points)[i].x);
        p4.setY(p4.y() + (*points)[i].y);
    }
    p4 /= (FaceAlignmentWidget::LANDMARK_EYEBROW_RIGHT_END - FaceAlignmentWidget::LANDMARK_EYEBROW_RIGHT_BEGIN + 1);

    p5 = QPointF(0, 0);
    for(int i = FaceAlignmentWidget::LANDMARK_NOSE_BEGIN; i <= FaceAlignmentWidget::LANDMARK_NOSE_END; i++)
    {
        p5.setX(p5.x() + (*points)[i].x);
        p5.setY(p5.y() + (*points)[i].y);
    }
    p5 /= (FaceAlignmentWidget::LANDMARK_NOSE_END - FaceAlignmentWidget::LANDMARK_NOSE_BEGIN + 1);

    p6 = QPointF(0, 0);
    for(int i = FaceAlignmentWidget::LANDMARK_EYE_MOUSE_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYE_MOUSE_END; i++)
    {
        p6.setX(p6.x() + (*points)[i].x);
        p6.setY(p6.y() + (*points)[i].y);
    }
    p6 /= (FaceAlignmentWidget::LANDMARK_EYE_MOUSE_END - FaceAlignmentWidget::LANDMARK_EYE_MOUSE_BEGIN + 1);
}

void DeformWidget::onTrackingTimer()
{
    QPointF leftEyePos(0.0f, 0.0f), rightEyePos(0.0f, 0.0f), leftBrowPos(0.0f, 0.0f), rightBrowPos(0.0f, 0.0f);
    QPointF nosePos(0.0f, 0.0f), mouthPos(0.0f, 0.0f);
    QPointF origLeftEyePos(0.0f, 0.0f), origRightEyePos(0.0f, 0.0f), origLeftBrowPos(0.0f, 0.0f), origRightBrowPos(0.0f, 0.0f);
    QPointF origNosePos(0.0f, 0.0f), origMouthPos(0.0f, 0.0f);
    evaluateKeyPoints(landmarks, leftEyePos, rightEyePos, leftBrowPos, rightBrowPos, nosePos, mouthPos);
    evaluateKeyPoints(originalLandmarks, origLeftEyePos, origRightEyePos, origLeftBrowPos, origRightBrowPos, origNosePos, origMouthPos);
    double seg11 = leftBrowPos.y() - (leftEyePos.y() + rightEyePos.y()) / 2.0;
    double seg12 = rightBrowPos.y() - (leftEyePos.y() + rightEyePos.y()) / 2.0;
    double seg2 = ((leftEyePos.y() + rightEyePos.y()) / 2.0) - nosePos.y();
    double seg3 = nosePos.y() - mouthPos.y();

    double origSeg11 = origLeftBrowPos.y() - ((origLeftEyePos.y() + origRightEyePos.y()) / 2.0);
    double origSeg12 = origRightBrowPos.y() - ((origLeftEyePos.y() + origRightEyePos.y()) / 2.0);
    double origSeg2 = ((origLeftEyePos.y() + origRightEyePos.y()) / 2.0) - origNosePos.y();
    double origSeg3 = origNosePos.y() - origMouthPos.y();

    double deformRatioLeftEye = ((seg11 / seg2) - (origSeg11 / origSeg2)) / (origSeg11 / origSeg2);
    double deformRatioRightEye = ((seg12 / seg2) - (origSeg12 / origSeg2)) / (origSeg12 / origSeg2);
    if(deformRatioLeftEye > 0.1) deformRatioLeftEye = 0.1;
    if(deformRatioLeftEye < -0.1) deformRatioLeftEye = -0.1;
    if(deformRatioRightEye > 0.1) deformRatioRightEye = 0.1;
    if(deformRatioRightEye < -0.1) deformRatioRightEye = -0.1;
    //deformRatio = 0.1;
    for(auto vh : cgalOutlineMaps["eye_left"])
    {
        deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()) - QPointF(0, 350 * deformRatioLeftEye));
    }
    for(auto vh : cgalOutlineMaps["eye_right"])
    {
        deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()) - QPointF(0, 350 * deformRatioRightEye), false);
    }

    double eLeftY = 0.0;
    double eRightY = 0.0;

    for(auto vh : cgalOutlineMaps["e_left"])
    {
        eLeftY += vh->point().y();
    }
    eLeftY /= cgalOutlineMaps["e_left"].size();

    for(auto vh : cgalOutlineMaps["e_right"])
    {
        eRightY += vh->point().y();
    }
    eRightY /= cgalOutlineMaps["e_right"].size();

    for(auto vh : cgalOutlineMaps["e_left"])
    {
        if(deformRatioLeftEye < 0)
        {
            deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()) - QPointF(0, (vh->point().y() - eLeftY) * 5 * -deformRatioLeftEye), false);
        }
        else
        {
            deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()), false);
        }
    }

    for(auto vh : cgalOutlineMaps["e_right"])
    {
        if(deformRatioRightEye < 0)
        {
            deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()) - QPointF(0, (vh->point().y() - eLeftY) * 5 * -deformRatioRightEye), false);
        }
        else
        {
            deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()), false);
        }
    }

    deformer->updateVertices();
    /*for(auto vh : cgalOutlineMaps["eye_right"])
    {
        deformer->moveVertex(vh, CGALUtil::toQtPointF(vh->point()) - QPointF(0, 350 * deformRatioLeftEye));
    }*/

    for(int i = FaceAlignmentWidget::LANDMARK_EYE_LEFT_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYE_LEFT_END; i++)
    {
        //leftEyePos += ;
    }
    for(int i = FaceAlignmentWidget::LANDMARK_EYE_RIGHT_BEGIN; i <= FaceAlignmentWidget::LANDMARK_EYE_RIGHT_END; i++)
    {
        leftEyePos;
    }
    repaint();
}

void DeformWidget::enbaleTracking(const std::vector<cv::Point2d> &originalLandmarks, const std::vector<cv::Point2d> &landmarks)
{
    this->originalLandmarks = &originalLandmarks;
    this->landmarks = &landmarks;
    trackingTimer = new QTimer(this);
    trackingTimer->setInterval(50);
    trackingTimer->setSingleShot(false);
    trackingTimer->start();
    connect(trackingTimer, SIGNAL(timeout()), this, SLOT(onTrackingTimer()));
}

void DeformWidget::disableTracking()
{
    this->originalLandmarks = this->landmarks = nullptr;
    trackingTimer->stop();
    delete trackingTimer;
}

void DeformWidget::changeColor(const QString &layerName)
{
    QImage *layer = layers[layerName];
    double averageHue = 0.0;
    int validPixelCount = 0;
    for(int i = 0; i < layer->width(); i++)
    {
        for(int j = 0; j < layer->height(); j++)
        {
            QColor c = layer->pixelColor(i, j);
            if(c.alphaF() > 0.5)
            {
                if(c.hsvHueF() >= 0.0)
                {
                    averageHue += c.hsvHueF();
                    validPixelCount++;
                }
            }
        }
    }
    averageHue /= validPixelCount;
    qDebug() << validPixelCount;
    qDebug() << averageHue;
    qDebug() << QColor::fromHsvF(averageHue, 1.0, 1.0);
    QColor newColor = QColorDialog::getColor(QColor::fromHsvF(averageHue, 1.0, 1.0));
    qDebug() << newColor;
    if(!newColor.isValid()) return;
    double hueDelta = newColor.hsvHueF() - averageHue;
    qDebug() << "Updateing layer...";
    for(int i = 0; i < layer->width(); i++)
    {
        for(int j = 0; j < layer->height(); j++)
        {
            QColor c = layer->pixelColor(i, j);
            if(c.alphaF() > 0.5)
            {
                if(c.hsvHueF() >= 0.0)
                {
                    double newHue = c.hsvHueF() + hueDelta;
                    if(newHue < 0.0) newHue += 1.0;
                    else if(newHue > 1.0) newHue -= 1.0;
                    c.setHsvF(newHue, c.hsvSaturationF(), c.valueF());
                    layer->setPixelColor(i, j, c);
                }
            }
        }
    }
    repaint();
}
