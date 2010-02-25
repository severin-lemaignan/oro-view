#ifndef EDGE_H
#define EDGE_H

#include <vector>

class NodeRelation;

class Edge
{
    std::vector<const NodeRelation*> relations;

public:
    Edge(const NodeRelation& rel);

    void addReferenceRelation(const NodeRelation& rel);

    bool coversRelation(const NodeRelation& rel);
};

#endif // EDGE_H
