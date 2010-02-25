#include <algorithm>
#include <boost/functional/hash.hpp>

#include "macros.h"
#include "oroview_exceptions.h"

#include "edge.h"
#include "noderelation.h"
#include "node.h"

using namespace std;
using namespace boost;

Edge::Edge(const NodeRelation& rel) : renderer(EdgeRenderer(
	hash_value(rel.from->getID() + rel.to->getID())))
{
    node1 = NULL;
    node2 = NULL;

    renderingDone = false;

    addReferenceRelation(rel);
}

bool Edge::coversRelation(const NodeRelation& rel) {
    return !(relations.end() == find(relations.begin(), relations.end(), &rel));
}

void Edge::addReferenceRelation(const NodeRelation& rel) {
    relations.push_back(&rel);

    if (node1 == NULL) {
	node1 = rel.from;
	node2 = rel.to;
    }
    else //check that the newly added relation is between the right nodes
	if (!(node1 == rel.from && node2 == rel.to) &&
	    !(node1 == rel.to && node2 == rel.from)) {
	    throw OroViewException("Trying to add a relation to an edge for the wrong nodes!");
	}
}

void Edge::render(){

    if (!renderingDone) {

#ifndef TEXT_ONLY
	renderer.renderAt();
#endif
	TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " rendered.");
	renderingDone = true;
    }
}
