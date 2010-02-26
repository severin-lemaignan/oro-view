#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "edge_renderer.h"

class Node;
class NodeRelation;

class Edge
{
    std::vector<const NodeRelation*> relations;

    Node* node1;
    Node* node2;

    void updateLength();

    vec2f spos;

    bool stepDone;
    bool renderingDone;

    EdgeRenderer renderer;

public:
    Edge(const NodeRelation& rel);

    float length;
    float spring_constant;
    float nominal_length;

    void addReferenceRelation(const NodeRelation& rel);

    bool coversRelation(const NodeRelation& rel);

    void resetRenderers();

    void step(float dt);
    void render();
};

#endif // EDGE_H
