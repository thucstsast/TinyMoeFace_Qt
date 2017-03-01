#ifndef FACEALIGNMENTWIDGET_H
#define FACEALIGNMENTWIDGET_H

#include <QWidget>

class QCamera;

class FaceAlignmentWidget : public QWidget
{
    Q_OBJECT
protected:
    QCamera *camera;
    void paintEvent(QPaintEvent * event) override;
public:
    explicit FaceAlignmentWidget(QWidget *parent = 0);
    void setCamera();
    void startCapture();
    void stopCapture();

signals:

public slots:
};

#endif // FACEALIGNMENTWIDGET_H
