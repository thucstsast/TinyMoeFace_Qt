#ifndef DEFORMWIDGET_H
#define DEFORMWIDGET_H

#include <QWidget>

class DeformWidget : public QWidget
{
    Q_OBJECT
protected:
    QImage *image = nullptr;
    void paintEvent(QPaintEvent * event) override;
public:
    explicit DeformWidget(QWidget *parent = 0);
    virtual bool loadMetadata(const QString& path);
    virtual bool loadImage(const QString& path);
    virtual void performEdgeDetect();

signals:

public slots:
};

#endif // DEFORMWIDGET_H
