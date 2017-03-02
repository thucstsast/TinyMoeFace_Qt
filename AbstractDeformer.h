#ifndef ABSTRACTDEFORMER_H
#define ABSTRACTDEFORMER_H

#include "CGALTriangulator.h"
#include <QVector>
#include <QSet>
#include <QMap>

class AbstractDeformer
{
protected:
    CDT &triangleMesh;
public:
    AbstractDeformer(CDT& triangleMesh);
    QMap<CDT::Vertex_handle, QPointF> constrainedVertices;
    virtual bool addConstrainedVertex(CDT::Vertex_handle vertexHandle);
    virtual bool removeConstrainedVertex(CDT::Vertex_handle vertexHandle);
    virtual const QMap<CDT::Vertex_handle, QPointF>& getConstrainedVerices();
    virtual void moveVertex(CDT::Vertex_handle vertexHandle, const QPointF& location);
    virtual void updateVertices() = 0;
};

#endif // ABSTRACTDEFORMER_H
