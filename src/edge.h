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

    vec2f spos;

    vec4f col1;
    vec4f col2;

    bool stepDone;
    bool renderingDone;

    EdgeRenderer renderer;

public:
    Edge(const NodeRelation& rel);

    void addReferenceRelation(const NodeRelation& rel);

    bool coversRelation(const NodeRelation& rel);

    void initializeNextStep();

    void step(float dt);
    void render();
};

#endif // EDGE_H
