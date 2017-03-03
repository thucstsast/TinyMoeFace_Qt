#ifndef PHYSICSDEFORMER_H
#define PHYSICSDEFORMER_H

#include "AbstractDeformer.h"

class PhysicsDeformer : public AbstractDeformer
{
public:
    PhysicsDeformer(CDT& triangleMesh);
    void updateVertices() override;
};

#endif // PHYSICSDEFORMER_H
