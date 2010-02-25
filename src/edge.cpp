#include <algorithm>
#include <boost/functional/hash.hpp>

#include "core/vectors.h"

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

    col1 = vec4f(0.3, 0.5, 0.7, 1.0);
    col1 = vec4f(0.7, 0.5, 0.3, 1.0);

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

void Edge::initializeNextStep(){
    stepDone = false;
    renderingDone = false;
}

void Edge::step(float dt){

    if(!stepDone) {

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

	renderer.updateSpline(pos1, col1, pos2, col2, spos);

	stepDone = true;
    }
}

void Edge::render(){

    if (!renderingDone) {

#ifndef TEXT_ONLY
	renderer.render();
#endif
	TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " rendered.");
	renderingDone = true;
    }
}
