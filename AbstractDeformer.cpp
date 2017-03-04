#include "AbstractDeformer.h"

AbstractDeformer::AbstractDeformer(CDT &triangleMesh)
    :triangleMesh(triangleMesh)
{
    for(auto i = triangleMesh.vertices_begin(); i != triangleMesh.vertices_end(); i++)
    {
        originalVertexPos[i] = CGALUtil::toQtPointF(i->point());
    }
    modifiedVertexPos = originalVertexPos;
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

void AbstractDeformer::moveVertex(CDT::Vertex_handle vertexHandle, const QPointF &location, bool update)
{
    constrainedVertices[vertexHandle] = location;
    if(update)
        updateVertices();
}

QPointF AbstractDeformer::getVertexPosition(CDT::Vertex_handle vertexHandle)
{
    return modifiedVertexPos[vertexHandle];
}
