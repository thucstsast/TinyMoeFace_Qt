#include <QDebug>
#include "SimpleDeformer.h"

SimpleDeformer::SimpleDeformer(CDT& triangleMesh)
    :AbstractDeformer(triangleMesh)
{

}

void SimpleDeformer::updateVertices()
{
    for(auto i = triangleMesh.vertices_begin(); i != triangleMesh.vertices_end(); i++)
    {
        Vertex_handle vertexHandle = i;
        if(constrainedVertices.contains(vertexHandle))
        {
            modifiedVertexPos[vertexHandle] = constrainedVertices[vertexHandle];
        }
        else
        {
            modifiedVertexPos[i] = originalVertexPos[i];
            /*int count = 0;
            for(auto j = triangleMesh.incident_vertices(i); j != triangleMesh.vertices_end(); j++)
            {
                count++;
            }
            qDebug() << count;*/
            for(auto j : constrainedVertices.keys())
            {
                auto deltaJ = constrainedVertices[j] - originalVertexPos[j];
                auto deltaVec = originalVertexPos[j] - originalVertexPos[i];
                auto dist2 = deltaVec.x() * deltaVec.x() + deltaVec.y() * deltaVec.y();
                modifiedVertexPos[i] += deltaJ * 0.5 * exp(- dist2 / 1000.0);//deltaJ * exp(-dist2 / 1000.0);
            }
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
