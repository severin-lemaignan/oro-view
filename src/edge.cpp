#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>

#include "core/vectors.h"

#include "macros.h"
#include "oroview_exceptions.h"

#include "edge.h"
#include "node_relation.h"
#include "node.h"
#include "graph.h"

using namespace std;
using namespace boost;

Edge::Edge(const NodeRelation& rel) : idNode1(rel.from->getID()), idNode2(rel.to->getID()), renderer(EdgeRenderer(
	hash_value(rel.from->getID() + rel.to->getID())))
{
    renderingDone = false;

//    addReferenceRelation(rel);

    spring_constant = INITIAL_SPRING_CONSTANT;
    nominal_length = NOMINAL_EDGE_LENGTH;

    length = 0.0;
}

//bool Edge::coversRelation(const NodeRelation& rel) {
//    //TODO: BUG HERE!! this report incorrectly some relation to be covered...
//    //return !(relations.end() == find(relations.begin(), relations.end(), &rel));
//    return false;
//}

//void Edge::addReferenceRelation(const NodeRelation& rel) {
//    relations.push_back(&rel);
//
//    if (node1 == NULL) {
//	node1 = rel.from;
//	node2 = rel.to;
//    }
//    else //check that the newly added relation is between the right nodes
//	if (!(node1 == rel.from && node2 == rel.to) &&
//	    !(node1 == rel.to && node2 == rel.from)) {
//	    throw OroViewException("Trying to add a relation to an edge for the wrong nodes!");
//	}
//}

//void Edge::removeReferenceRelation(const NodeRelation& rel) {
//    remove(relations.begin(), relations.end(), &rel);
//
//    if (relations.size() == 0)
//	throw OroViewException("Removed the last relation of an edge!");
//}

void Edge::resetRenderers(){
    stepDone = false;
    renderingDone = false;
}

//int Edge:: countRelations() const {
//    return relations.size();
//}

//bool Edge::hasOutboundConnectionFrom(const Node* node) const{
//    BOOST_FOREACH(const NodeRelation* r, relations) {
//	if (r->from == node)
//	    return true;
//    }
//    return false;
//}

void Edge::step(Graph& g, float dt){

    if(!stepDone) {

	updateLength(g);

#ifndef TEXT_ONLY

	Node& node1 = g.getNode(idNode1);
	Node& node2 = g.getNode(idNode2);
	
	const vec2f& pos1 = node1.pos;
	const vec2f& pos2 = node2.pos;

	//update the spline point
	vec2f td = (pos2 - pos1) * 0.5;

	vec2f mid = pos1 + td;// - td.perpendicular() * pos.normal();// * 10.0;

	vec2f delta = (mid - spos);

	//dont let spos get more than half the length of the distance behind
	if(delta.length2() > td.length2()) {
	    spos += delta.normal() * (delta.length() - td.length());
	}

	spos += delta * min(1.0, dt * 2.0);

	renderer.update(pos1, node1.renderer.col, pos2, node2.renderer.col, spos);

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

void Edge::updateLength(Graph& g) {
    //TODO: optimisation by using length2 here?
    length = (g.getNode(idNode1).pos -  g.getNode(idNode2).pos).length();
}

const string& Edge::getId1() const {
    return idNode1;
}

const string& Edge::getId2() const {
    return idNode2;
}
