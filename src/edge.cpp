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
    node1(rel.from),
    node2(rel.to),
    rel_type(rel.type),
    renderer(EdgeRenderer(
            hash_value(rel.from->getID() + rel.to->getID()),
            label,
            rel_type
            ))
{
    //    addReferenceRelation(rel);

    spring_constant = INITIAL_SPRING_CONSTANT;
    nominal_length = NOMINAL_EDGE_LENGTH;

    length = 0.0;
}

void Edge::step(Graph& g, float dt){

    updateLength();

#ifndef TEXT_ONLY

    const vec2f& pos1 = node1->pos;
    const vec2f& pos2 = node2->pos;

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
                (node1->selected ? NODE_SIZE * SELECT_SIZE_FACTOR : NODE_SIZE) / 2 + 2
                );

    vec2f out_of_node2_distance = td * (
                (node2->selected ? NODE_SIZE * SELECT_SIZE_FACTOR : NODE_SIZE) / 2 + 2
                );

    (node1->selected || node2->selected) ? renderer.selected = true : renderer.selected = false;
    //Update the age of the node renderer
    renderer.increment_idle_time(dt);

    renderer.update(pos1 + out_of_node1_distance , node1->renderer.col,
                    pos2  - out_of_node2_distance , node2->renderer.col, spos);

#endif
    //TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " updated.");

}

void Edge::render(rendering_mode mode, OroView& env){



#ifndef TEXT_ONLY
    int distance = std::min(node1->distance_to_selected, node2->distance_to_selected);
    if (distance >= MAX_NODE_LEVELS - 1) return;
    renderer.draw(mode, env, distance);
#endif
    //TRACE("Edge between " << node1->getID() << " and " << node2->getID() << " rendered.");

}

void Edge::updateLength() {
    //TODO: optimisation by using length2 here?
    length = (node1->pos -  node2->pos).length();
}

const string& Edge::getId1() const {
    return node1->getID();
}

const string& Edge::getId2() const {
    return node2->getID();
}
