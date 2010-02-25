#include <algorithm>

#include "edge.h"

Edge::Edge(const NodeRelation& rel)
{
    addReferenceRelation(rel);
}

bool Edge::coversRelation(const NodeRelation& rel) {
    return !(relations.end() == find(relations.begin(), relations.end(), &rel));
}

void Edge::addReferenceRelation(const NodeRelation& rel) {
    relations.push_back(&rel);
}
