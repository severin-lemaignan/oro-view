/*
    Copyright (c) 2010 Séverin Lemaignan (slemaign@laas.fr)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

Edge::Edge(const NodeRelation& rel, const string& label) :
    idNode1(rel.from->getID()),
    idNode2(rel.to->getID()),
    renderer(EdgeRenderer(
            hash_value(rel.from->getID() + rel.to->getID()),
            label
            ))
{

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

    td.normalize();

    vec2f out_of_node1_distance = td * (
                (node1.selected ? NODE_SIZE * SELECT_SIZE_FACTOR : NODE_SIZE) / 2 + 2
                );

    vec2f out_of_node2_distance = td * (
                (node2.selected ? NODE_SIZE * SELECT_SIZE_FACTOR : NODE_SIZE) / 2 + 2
                );

    //Update the age of the node renderer
    renderer.increment_idle_time(dt);

    renderer.update(pos1 + out_of_node1_distance , node1.renderer.col,
                    pos2  - out_of_node2_distance , node2.renderer.col, spos);

#endif
    //TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " updated.");

}

void Edge::render(rendering_mode mode, OroView& env){



#ifndef TEXT_ONLY
    renderer.draw(mode, env);
#endif
    //TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " rendered.");

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
