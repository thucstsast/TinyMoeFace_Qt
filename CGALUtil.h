#ifndef CGALUTIL_H
#define CGALUTIL_H

#include <QPointF>

#include "CGALTriangulator.h"

class CGALUtil
{
public:
    template<class T>
    static QPointF toQtPointF(const T& cgalPoint);
};

template<class T>
QPointF CGALUtil::toQtPointF(const T &cgalPoint)
{
    return QPointF(cgalPoint.x(), cgalPoint.y());
}

#endif // CGALUTIL_H
