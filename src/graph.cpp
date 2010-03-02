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
}


void Graph::step(float dt) {

    BOOST_FOREACH(Edge& e, edges) {
	e.step(*this, dt);
    }

    BOOST_FOREACH(NodeMap::value_type& n, nodes) {

	n.second.step(*this, dt);
    }
}

void Graph::render(bool complete) {

    if (complete) {
	BOOST_FOREACH(Edge& e, edges) {
	    e.render();
	}
    }

    BOOST_FOREACH(NodeMap::value_type& n, nodes) {
	n.second.render(complete);
    }
}

void Graph::resetRenderers() {

    BOOST_FOREACH(NodeMap::value_type& n, nodes) {
	n.second.resetRenderers();
    }

    BOOST_FOREACH(Edge& e, edges) {
	e.resetRenderers();
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


Node& Graph::addNode(const string& id) {

    //I'm doing 2 !! copies of Node, here??
    pair<NodeMap::iterator, bool> res = nodes.insert(make_pair(hash_value(id),Node(id)));

    if ( ! res.second )
	TRACE("Didn't add node " << id << " because it already exists.");
    else
	TRACE("Added node " << id);

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
	TRACE("Leaving immediately: from == to");
	return;
    }

   //so now we are confident that there's no edge we can reuse. Let's create a new one.
    if (getEdgesBetween(from, to).size() == 0)
	edges.push_back(Edge(rel));


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

vec2f Graph::hookeAttractionFor(const Node& node) const {

     vec2f force(0.0, 0.0);

    //TODO: a simple optimization can be to compute Coulomb force
    //at the same time than Hooke force when possible -> one
    // less distance computation (not sure it makes a big difference)

    BOOST_FOREACH(const Edge* e, getEdgesFor(node)) {
	const Node& n_tmp = getConstNode(e->getId1());

	//Retrieve the node at the edge other extremity
	const Node& n2 = ( (&n_tmp == &node) ? n_tmp : getConstNode(e->getId2()) );

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


