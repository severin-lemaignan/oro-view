#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "edge_renderer.h"
#include "styles.h"

class Graph;
class NodeRelation;

class Edge
{
    std::string idNode1;
    std::string idNode2;

    void updateLength(Graph& g);

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

    void step(Graph& g, float dt);
    void render(rendering_mode mode);

    const std::string& getId1() const;
    const std::string& getId2() const;

};

#endif // EDGE_H
