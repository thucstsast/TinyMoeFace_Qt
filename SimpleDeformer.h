#ifndef SIMPLECDEFORMER_H
#define SIMPLECDEFORMER_H

#include "AbstractDeformer.h"

class SimpleDeformer : public AbstractDeformer
{
public:
    SimpleDeformer(CDT& triangleMesh);
    void updateVertices() override;
    //QMap<CDT::Vertex_handle, QSet<CDT::VertexHandle>> adjacantVertices;
};

#endif // SIMPLECDEFORMER_H
