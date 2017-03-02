#include "AbstractDeformer.h"

AbstractDeformer::AbstractDeformer(CDT &triangleMesh)
    :triangleMesh(triangleMesh)
{

}

bool AbstractDeformer::addConstrainedVertex(CDT::Vertex_handle vertexHandle)
{
    if(constrainedVertices.contains(vertexHandle))
    {
        return false;
    }
    constrainedVertices.insert(vertexHandle, CGALUtil::toQtPointF(vertexHandle->point()));
    return true;
}

bool AbstractDeformer::removeConstrainedVertex(CDT::Vertex_handle vertexHandle)
{
    return constrainedVertices.remove(vertexHandle);
}

const QMap<CDT::Vertex_handle, QPointF> &AbstractDeformer::getConstrainedVerices()
{
    return constrainedVertices;
}

void AbstractDeformer::moveVertex(CDT::Vertex_handle vertexHandle, const QPointF &location)
{
    constrainedVertices[vertexHandle] = location;
    updateVertices();
}
