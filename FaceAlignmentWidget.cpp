#include <QPainter>
#include "FaceAlignmentWidget.h"

FaceAlignmentWidget::FaceAlignmentWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(100, 100);
}

void FaceAlignmentWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.drawEllipse(50,50,50,50);
}

void FaceAlignmentWidget::setCamera()
{

}

void FaceAlignmentWidget::startCapture()
{

}

void FaceAlignmentWidget::stopCapture()
{

}
