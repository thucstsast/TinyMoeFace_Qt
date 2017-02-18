#include <QVector2D>
#include "ARAPDeformer.h"

ARAPDeformer::ARAPDeformer(CDT &triangleMesh)
    :triangleMesh(triangleMesh),
      G(triangleMesh.number_of_vertices() * 2, triangleMesh.number_of_vertices() * 2)
{

}

void ARAPDeformer::prepare()
{
    buildVertexMap();
}

void ARAPDeformer::buildVertexMap()
{
    freeVertexCount = 0;
    for(auto i = triangleMesh.vertices_begin(); i != triangleMesh.vertices_begin(); i++)
    {
        if(!constrainedVertices.contains(i))
        {
            freeVertexCount++;
            indexToVertex.append(i);
            vertexToIndex.insert(i, indexToVertex.size() - 1);
        }
    }
    for(auto i = triangleMesh.vertices_begin(); i != triangleMesh.vertices_begin(); i++)
    {
        if(constrainedVertices.contains(i))
        {
            indexToVertex.append(i);
            vertexToIndex.insert(i, indexToVertex.size() - 1);
        }
    }
}

//Solve equation (1)
void ARAPDeformer::getLocalCoordinate(const QPointF &v0, const QPointF &v1, const QPointF &v2, float &x, float &y)
{
    QVector2D v0v1(v1 - v0);
    QVector2D v0v1rotate90(-v0v1.y(), v0v1.x());
    QVector2D v0v2(v2 - v0);
    float v0v1Length = v0v1.length();
    float v0v2Length = v0v2.length();
    x = QVector2D::dotProduct(v0v2, v0v1) / v0v2Length / v0v1Length;
    y = QVector2D::dotProduct(v0v2, v0v1rotate90) / v0v2Length / v0v1Length;
}

void ARAPDeformer::addStep1ErrorForTriangleEdge(const QPointF &v0, const QPointF &v1, const QPointF &v2, const int v0Index, const int v1Index, const int v2Index)
{
    float x, y;
    getLocalCoordinate(v0, v1, v2, x, y);
    auto v0IndexX = v0Index * 2;
    auto v0IndexY = v0Index * 2 + 1;
    auto v1IndexX = v1Index * 2;
    auto v1IndexY = v1Index * 2 + 1;
    auto v2IndexX = v2Index * 2;
    auto v2IndexY = v2Index * 2 + 1;

    //TODO: Add credit.
    G(v0IndexX, v0IndexX) += 1 - 2*x + x*x + y*y;
    G(v0IndexX, v1IndexX) += 2*x - 2*x*x - 2*y*y;
    G(v0IndexX, v1IndexY) += 2*y;
    G(v0IndexX, v2IndexX) += -2 + 2*x;
    G(v0IndexX, v2IndexY) += -2 * y;

    G(v0IndexY, v0IndexY) += 1 - 2*x + x*x + y*y;
    G(v0IndexY, v1IndexX) += -2*y;
    G(v0IndexY, v1IndexY) += 2*x - 2*x*x - 2*y*y;
    G(v0IndexY, v2IndexX) += 2*y;
    G(v0IndexY, v2IndexY) += -2 + 2*x;

    G(v1IndexX, v1IndexX) += x*x + y*y;
    G(v1IndexX, v2IndexX) += -2*x;
    G(v1IndexX, v2IndexY) += 2*y;

    G(v1IndexY, v1IndexY) += x*x + y*y;
    G(v1IndexY, v2IndexX) += -2*y;
    G(v1IndexY, v2IndexY) += -2*x;

    G(v2IndexX, v2IndexX) += 1;
    G(v2IndexY, v2IndexY) += 1;
}

void ARAPDeformer::addStep1ErrorForTriangle(const QPointF &v0, const QPointF &v1, const QPointF &v2, const int v0Index, const int v1Index, const int v2Index)
{
    addStep1ErrorForTriangleEdge(v0, v1, v2, v0Index, v1Index, v2Index);
    addStep1ErrorForTriangleEdge(v1, v2, v0, v1Index, v2Index, v0Index);
    addStep1ErrorForTriangleEdge(v2, v0, v1, v2Index, v0Index, v1Index);
}

