#ifndef DEFORMWIDGET_H
#define DEFORMWIDGET_H

#include "CGALTriangulator.h"
#include <QMap>
#include <QWidget>
#include "FaceOutlineReader.h"
#include <QSet>

class AbstractDeformer;

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

    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
public:
    explicit DeformWidget(QWidget *parent = 0);
    virtual bool loadMetadata(const QString& path);
    virtual bool loadImage(const QString& path);
    virtual void performEdgeDetect();
    virtual void enbaleTracking();
    virtual void disableTracking();

signals:

public slots:
    virtual void reset();
};

#endif // DEFORMWIDGET_H
