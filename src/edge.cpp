#include <algorithm>
#include <boost/functional/hash.hpp>

#include "core/vectors.h"

#include "macros.h"
#include "oroview_exceptions.h"

#include "edge.h"
#include "node_relation.h"
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

    spring_constant = INITIAL_SPRING_CONSTANT;
    nominal_length = NOMINAL_EDGE_LENGTH;
    updateLength();
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

void Edge::removeReferenceRelation(const NodeRelation& rel) {
    remove(relations.begin(), relations.end(), &rel);

    if (relations.size() == 0)
	throw OroViewException("Removed the last relation of an edge!");
}

void Edge::resetRenderers(){
    stepDone = false;
    renderingDone = false;
}

void Edge::step(float dt){

    if(!stepDone) {

	updateLength();

#ifndef TEXT_ONLY

	vec2f& pos1 = node1->pos;
	vec2f& pos2 = node2->pos;

	//update the spline point
	vec2f td = (pos2 - pos1) * 0.5;

	vec2f mid = pos1 + td;// - td.perpendicular() * pos.normal();// * 10.0;

	vec2f delta = (mid - spos);

	//dont let spos get more than half the length of the distance behind
	if(delta.length2() > td.length2()) {
	    spos += delta.normal() * (delta.length() - td.length());
	}

	spos += delta * min(1.0, dt * 2.0);

	renderer.update(pos1, node1->renderer.col, pos2, node2->renderer.col, spos);

#endif
	//TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " updated.");
	stepDone = true;
    }
}

void Edge::render(){

    if (!renderingDone) {

#ifndef TEXT_ONLY
	renderer.render();
#endif
	//TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " rendered.");
	renderingDone = true;
    }
}

void Edge::updateLength() {
    //TODO: optimisation by using length2 here?
    length = (node1->pos - node2->pos).length();

    //TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " has length " << length);
}
