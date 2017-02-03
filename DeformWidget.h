#ifndef DEFORMWIDGET_H
#define DEFORMWIDGET_H

#include <QMap>
#include <QWidget>

#include "FaceOutlineReader.h"

class DeformWidget : public QWidget
{
    Q_OBJECT
protected:
    QMap<QString, QImage*> layers;
    QStringList layerFileNames;
    QStringList layerOrder;
    QImage *image = nullptr;
    void paintEvent(QPaintEvent * event) override;
    FaceOutlineReader outlineReader;
public:
    explicit DeformWidget(QWidget *parent = 0);
    virtual bool loadMetadata(const QString& path);
    virtual bool loadImage(const QString& path);
    virtual void performEdgeDetect();

signals:

public slots:
};

#endif // DEFORMWIDGET_H
