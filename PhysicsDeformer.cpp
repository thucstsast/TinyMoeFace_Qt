#include <QDebug>
#include "PhysicsDeformer.h"

PhysicsDeformer::PhysicsDeformer(CDT& triangleMesh)
    :AbstractDeformer(triangleMesh)
{

}

void PhysicsDeformer::updateVertices()
{
    for(auto i = triangleMesh.vertices_begin(); i != triangleMesh.vertices_end(); i++)
    {
        Vertex_handle vertexHandle = i;
        if(constrainedVertices.contains(i))
        {
            auto &qtPoint = constrainedVertices[i];
            triangleMesh.move_if_no_collision(vertexHandle, CDT::Point(qtPoint.x(), qtPoint.y()));
        }
        /*int count = 0;
        for(auto j = triangleMesh.incident_vertices(i); j != triangleMesh.vertices_end(); j++)
        {
            count++;
        }
        qDebug() << count;*/
        /*Vertex_handle p = i;
        if(cgalOutlinesSet.contains(p))
        {
            painter.setBrush(QBrush(Qt::red));
        }
        else
        {
            painter.setBrush(QBrush(Qt::yellow));
        }
        painter.drawEllipse(CGALUtil::toQtPointF(p->point()), 2, 2);*/
    }
}
