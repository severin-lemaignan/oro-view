#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "edge_renderer.h"

class Node;
class NodeRelation;

class Edge
{
    std::vector<const NodeRelation*> relations;

    const Node* node1;
    const Node* node2;

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

    //void addReferenceRelation(const NodeRelation& rel);
    //void removeReferenceRelation(const NodeRelation& rel);

    //bool coversRelation(const NodeRelation& rel);

    //int countRelations() const;
    //bool hasOutboundConnectionFrom(const Node* node) const;

    void resetRenderers();

    void step(float dt);
    void render();
};

#endif // EDGE_H
