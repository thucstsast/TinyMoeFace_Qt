#ifndef DEFORMWIDGET_H
#define DEFORMWIDGET_H

#include <opencv/cv.hpp>
#include "CGALTriangulator.h"
#include <QMap>
#include <QWidget>
#include "FaceOutlineReader.h"
#include <QSet>

class AbstractDeformer;
class QTimer;

class DeformWidget : public QWidget
{
    Q_OBJECT
protected:
    CDT cdt;
    Vertex_handle selectedVertex;
    bool mousePressed = false;
    bool hasSelectedVertex = false;
    AbstractDeformer *deformer;
    int imageWidth = -1;
    int imageHeight = -1;
    QMap<QString, QImage*> layers;
    QStringList layerFileNames;
    QStringList layerOrder;
    QImage *image = nullptr;
    void paintEvent(QPaintEvent * event) override;
    FaceOutlineReader outlineReader;
    QMap<QString, QVector<Vertex_handle>> cgalOutlines;
    QSet<Vertex_handle> cgalOutlinesSet;
    QMap<QString, QSet<Vertex_handle> > cgalOutlineMaps;

    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    //Landmarks
    const std::vector<cv::Point2d>* originalLandmarks = nullptr;
    const std::vector<cv::Point2d>* landmarks = nullptr;
    QTimer *trackingTimer = nullptr;
public:
    explicit DeformWidget(QWidget *parent = 0);
    virtual bool loadMetadata(const QString& path);
    virtual bool loadImage(const QString& path);
    virtual void performEdgeDetect();
    void evaluateKeyPoints(const std::vector<cv::Point2d> *points,
            QPointF&, QPointF&, QPointF&, QPointF&, QPointF&, QPointF&);
    virtual void enbaleTracking(const std::vector<cv::Point2d>& originalLandmarks, const std::vector<cv::Point2d>& landmarks);
    virtual void disableTracking();

signals:

public slots:
    virtual void reset();
    virtual void onTrackingTimer();
};

#endif // DEFORMWIDGET_H
