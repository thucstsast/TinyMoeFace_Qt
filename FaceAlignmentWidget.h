#ifndef FACEALIGNMENTWIDGET_H
#define FACEALIGNMENTWIDGET_H

#include <QWidget>

class FaceAlignmentWidget : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent * event) override;
public:
    explicit FaceAlignmentWidget(QWidget *parent = 0);

signals:

public slots:
};

#endif // FACEALIGNMENTWIDGET_H
