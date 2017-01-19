#include <QPainter>
#include "DeformWidget.h"

DeformWidget::DeformWidget(QWidget *parent) : QWidget(parent)
{

}

void DeformWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    if(image == nullptr)
    {
        return;
    }
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
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
