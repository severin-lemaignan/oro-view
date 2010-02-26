#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>

#include "macros.h"

#include "graph.h"
#include "node.h"
#include "noderelation.h"


using namespace std;
using namespace boost;

Node::Node(string id) : id(id), renderer(NodeRenderer(hash_value(id)))
{
    pos = vec2f(100.0 * (float)rand()/RAND_MAX - 50 , 100 * (float)rand()/RAND_MAX - 50);

    //speed = vec2f(40.0 * (float)rand()/RAND_MAX - 20 , 40.0 * (float)rand()/RAND_MAX - 20);
    speed = vec2f(0.0, 0.0);

    mass = INITIAL_MASS;
    damping = INITIAL_DAMPING;

    resetRenderers();
}

bool Node::operator< (const Node& node2) const {
    return id < node2.getID();
}

const string& Node::getID() const {
    return id;
}

std::vector<NodeRelation>& Node::getRelations() {
    return relations;
}

void Node::addRelation(Node& to, const relation_type type, const std::string& label) {
    relations.push_back(NodeRelation(this, &to, type, label));

    TRACE("Added relation from " << id << " to " << to.getID());

    //Ask the graph to create the edge for this relation. If an edge already exist between the two nodes,
    //it will be reused.
    Graph::getInstance()->addEdge(relations.back());
}

vector<const NodeRelation*> Node::getRelationTo(Node& node) const {

    vector<const NodeRelation*> res;

    BOOST_FOREACH(const NodeRelation& rel, relations) {
		if (rel.to == &node)
			res.push_back(&rel);
	}
    return res;
}

void Node::resetRenderers(){
    stepDone = false;
    renderingDone = false;
}

vec2f Node::coulombRepulsionWith(const Node& node) const {
    return vec2f(0.0, 0.0);
}

vec2f Node::hookeAttractionWith(const NodeRelation& rel) const {
    Edge& e = *(rel.edge_p);
    float f = - e.spring_constant * (e.length - e.nominal_length);

    vec2f delta = rel.to->pos - pos;

    return project(f, delta);
}

vec2f Node::project(float force, vec2f d) const {
    //we need to project this force on x and y
    //-> Fx = F.cos(arctan(Dy/Dx)) = F/sqrt(1-(Dy/Dx)^2)
    //-> Fy = F.sin(arctan(Dy/Dx)) = F.(Dy/Dx)/sqrt(1-(Dy/Dx)^2)
    vec2f res(0.0, 0.0);

    if (d.y == 0) {
	res.x = force;
	return res.x;
    }

    if (d.x == 0) {
	res.y = force;
	return res.y;
    }

    float dydx = d.y/d.x;
    float sqdydx = 1/sqrt(1 + dydx * dydx);

    TRACE("d.x:" << d.x << " d.y:" << d.y << " 1/sqrt(1 - dxdy * dxdy):" << sqdydx);

    res.x = force * sqdydx;
    if (d.x > 0.0) res.x = - res.x;
    res.y = force * sqdydx * abs(dydx);
    if (d.y > 0.0) res.y = - res.y;

    TRACE("res.x:" << res.x << " res.y:" << res.y);
    return res;
}

void Node::updateKineticEnergy() {
    kinetic_energy += mass * speed.length2();
}

void Node::step(float dt){

    if(!stepDone) {

	/** Compute here the new position of the node **/

	// Algo from Wikipedia -- http://en.wikipedia.org/wiki/Force-based_layout

	vec2f force = vec2f(0.0, 0.0);

//	BOOST_FOREACH(Node& node, nodes) {
//	    force += coulombRepulsionWith(node);
//	}

	BOOST_FOREACH(NodeRelation& rel, relations) {
	    force += hookeAttractionWith(rel);
	}
	TRACE("Force after applying springs attraction: Fx=" << force.x << ", Fy= " << force.y);

	speed = (speed + force * dt) * damping;

	updateKineticEnergy();

	//Check we have enough energy to move :)
	if (kinetic_energy > MIN_KINETIC_ENERGY) {
	    pos += speed * dt;
	}


	TRACE("Step computed for " << id << ". Speed is " << speed.x << ", " << speed.y << " (energy: " << kinetic_energy << ").");
	stepDone = true;
    }
}

void Node::render(bool complete){

    if (!renderingDone) {
#ifndef TEXT_ONLY
	renderer.renderAt(pos);
#endif
	TRACE("Node " << id << " rendered.");
	renderingDone = true;
    }
}
