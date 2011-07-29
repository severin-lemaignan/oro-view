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

#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>

#include <iterator>
#include <utility>

#include "macros.h"
#include "graph.h"
#include "edge.h"
#include "node_relation.h"

using namespace std;
using namespace boost;

Graph::Graph()
{
    selectedNode = NULL;
}


void Graph::step(float dt) {

    BOOST_FOREACH(Edge& e, edges) {
        e.step(*this, dt);
    }

    BOOST_FOREACH(NodeMap::value_type& n, nodes) {

        n.second.step(*this, dt);
    }
}

void Graph::render(rendering_mode mode, OroView& env, bool debug) {

    // Renders edges
    BOOST_FOREACH(Edge& e, edges) {
        e.render(mode, env);
    }

    // Renders nodes
    BOOST_FOREACH(NodeMap::value_type& n, nodes) {
        n.second.render(mode, env, debug);
    }

}

const Graph::NodeMap& Graph::getNodes() const {
    return nodes;
}

Node& Graph::getNode(const string& id) {

    NodeMap::iterator it = nodes.find(hash_value(id));

    if (it == nodes.end())
        throw OroViewException("Node " + id + " not found");

    return it->second;

}

const Node& Graph::getConstNode(const string& id) const {

    NodeMap::const_iterator it = nodes.find(hash_value(id));

    if (it == nodes.end())
        throw OroViewException("Node " + id + " not found");

    return it->second;

}

Node* Graph::getNodeByTagID(int tagid) {

    NodeMap::iterator it = nodes.find(tagid);

    if (it == nodes.end())
        return NULL;

    return &(it->second);

}

Node& Graph::getRandomNode() {
    NodeMap::iterator it = nodes.begin();
    advance( it, rand()%nodes.size());
    return it->second;
}


Node& Graph::addNode(const string& id, const string& label, const Node* neighbour, node_type type) {

    pair<NodeMap::iterator, bool> res;

    //TODO: I'm doing 2 !! copies of Node, here??
    res = nodes.insert(make_pair(hash_value(id),Node(id, label, neighbour, type)));

    if ( ! res.second )
        TRACE("Didn't add node " << id << " because it already exists.");
    else
        TRACE("Added node " << id);

    updateDistances();

    return getNode(id);
}

/**
Ask the graph to create the edge for this relation. If an edge already exist between the two nodes,
it will be reused.
*/
void Graph::addEdge(Node& from, Node& to, const relation_type type, const std::string& label) {

    NodeRelation& rel = from.addRelation(to, type, label);

    //Don't add an edge if the relation is between the same node.
    //It could be actually useful, but it provokes a segfault somewhere :-/
    if (&from == &to) {
        TRACE("Leaving immediately because of strange segfault: from == to");
        return;
    }

    if (getEdgesBetween(from, to).size() == 0)
        //so now we are confident that there's no edge we can reuse. Let's create a new one.
        edges.push_back(Edge(rel, label));


    return;
}



vector<const Edge*>  Graph::getEdgesFor(const Node& node) const{
    vector<const Edge*> res;

    BOOST_FOREACH(const Edge& e, edges) {
        if (e.getId1() == node.getID() ||
                e.getId2() == node.getID())
            res.push_back(&e);
    }
    return res;
}

vector<Edge*>  Graph::getEdgesBetween(const Node& node1, const Node& node2){
    vector<Edge*> res;

    BOOST_FOREACH(Edge& e, edges) {
        if ((e.getId1() == node1.getID() && e.getId2() == node2.getID()) ||
                (e.getId1() == node2.getID() && e.getId2() == node1.getID()))
            res.push_back(&e);
    }
    return res;
}

void Graph::updateDistances() {

    // No node selected, set all distance to -1
    if (selectedNode == NULL) {
        // Renders nodes
        BOOST_FOREACH(NodeMap::value_type& n, nodes) {
            n.second.distance_to_selected = -1;
        }

        return;
    }
    cout << "Toto" << endl;
    //Else, start from the selected node
    recurseUpdateDistances(selectedNode, NULL, 0);

}

void Graph::recurseUpdateDistances(Node* node, Node* parent, int distance) {
    node->distance_to_selected = distance;
    //cout << node->getID() << " at distance " << distance << endl;
    TRACE("Node " << node->getID() << " is at " << distance << " nodes from selected");

    BOOST_FOREACH(Node* n, node->getConnectedNodes()){
        if (n != parent) recurseUpdateDistances(n, node, distance + 1);
    }
}

int Graph::nodesCount() {
    return nodes.size();
}

int Graph::edgesCount() {
    return edges.size();
}

vec2f Graph::coulombRepulsionFor(const Node& node) const {

    vec2f force(0.0, 0.0);

    //TODO: a simple optimization can be to compute Coulomb force
    //at the same time than Hooke force when possible -> one
    // less distance computation (not sure it makes a big difference)

    BOOST_FOREACH(const NodeMap::value_type& nm, nodes) {
        const Node& n = nm.second;
        if (&n != &node) {
            vec2f delta = n.pos - node.pos;

            //Coulomb repulsion force is in 1/r^2
            float len = delta.length2();
            if (len < 0.01) len = 0.01; //avoid dividing by zero

            float f = COULOMB_CONSTANT * n.charge * node.charge / len;

            force += project(f, delta);
        }
    }

    return force;

}

vec2f Graph::coulombRepulsionAt(const vec2f& pos) const {

    vec2f force(0.0, 0.0);

    //TODO: a simple optimization can be to compute Coulomb force
    //at the same time than Hooke force when possible -> one
    // less distance computation (not sure it makes a big difference)

    BOOST_FOREACH(const NodeMap::value_type& nm, nodes) {
        const Node& n = nm.second;

        vec2f delta = n.pos - pos;

        //Coulomb repulsion force is in 1/r^2
        float len = delta.length2();
        if (len < 0.01) len = 0.01; //avoid dividing by zero

        float f = COULOMB_CONSTANT * n.charge * INITIAL_CHARGE / len;

        force += project(f, delta);
    }

    return force;

}

vec2f Graph::hookeAttractionFor(const Node& node) const {

    vec2f force(0.0, 0.0);

    //TODO: a simple optimization can be to compute Coulomb force
    //at the same time than Hooke force when possible -> one
    // less distance computation (not sure it makes a big difference)

    BOOST_FOREACH(const Edge* e, getEdgesFor(node)) {
        const Node& n_tmp = getConstNode(e->getId1());

        //Retrieve the node at the edge other extremity
        const Node& n2 = ( (&n_tmp != &node) ? n_tmp : getConstNode(e->getId2()) );

        TRACE("\tComputing Hooke force from " << node.getID() << " to " << n2.getID());

        vec2f delta = n2.pos - node.pos;

        float f = - e->spring_constant * (e->length - e->nominal_length);

        force += project(f, delta);
    }

    return force;
}

vec2f Graph::project(float force, const vec2f& d) const {
    //we need to project this force on x and y
    //-> Fx = F.cos(arctan(Dy/Dx)) = F/sqrt(1-(Dy/Dx)^2)
    //-> Fy = F.sin(arctan(Dy/Dx)) = F.(Dy/Dx)/sqrt(1-(Dy/Dx)^2)
    vec2f res(0.0, 0.0);

    TRACE("\tForce: " << force << " - Delta: (" << d.x << ", " << d.y << ")");

    if (d.y == 0.0) {
        res.x = force;
        return res;
    }

    if (d.x == 0.0) {
        res.y = force;
        return res;
    }

    float dydx = d.y/d.x;
    float sqdydx = 1/sqrt(1 + dydx * dydx);

    res.x = force * sqdydx;
    if (d.x > 0.0) res.x = - res.x;
    res.y = force * sqdydx * abs(dydx);
    if (d.y > 0.0) res.y = - res.y;

    TRACE("\t-> After projection: Fx=" << res.x << ", Fy=" << res.y);

    return res;
}


