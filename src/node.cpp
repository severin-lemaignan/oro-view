#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>

#include <algorithm>

#include "macros.h"

#include "oroview.h"
#include "graph.h"
#include "node.h"
#include "node_relation.h"


using namespace std;
using namespace boost;

Node::Node(string id) : id(id), renderer(NodeRenderer(hash_value(id), id))
{
    pos = vec2f(100.0 * (float)rand()/RAND_MAX - 50 , 100 * (float)rand()/RAND_MAX - 50);

    //speed = vec2f(40.0 * (float)rand()/RAND_MAX - 20 , 40.0 * (float)rand()/RAND_MAX - 20);
    speed = vec2f(0.0, 0.0);

    mass = INITIAL_MASS;
    damping = INITIAL_DAMPING;

    charge = INITIAL_CHARGE;

    resetRenderers();

    selected = false;
}

bool Node::operator< (const Node& node2) const {
    return id < node2.getID();
}

const string& Node::getID() const {
    return id;
}

void Node::setColour(vec4f col) {
    renderer.setColour(col);
}

std::vector<NodeRelation>& Node::getRelations() {
    return relations;
}

NodeRelation& Node::addRelation(Node& to, const relation_type type, const std::string& label) {

    //When adding a new relation, we create as well an initial UNDEFINED reciprocal relation if
    //the destination node has no back relation.
    //And before adding a relation, we check there is no existing UNDEFINED relation.
    //If it's the case, we can safely replace it by a well defined relation.

//    if (type == UNDEFINED) {
//	//Add an undefined relation and return.
//	relations.push_back(NodeRelation(this, &to, UNDEFINED, ""));
//	TRACE("Added UNDEFINED back relation to " << to.getID());
//	return;
//    }

    vector<const NodeRelation*> rels = getRelationTo(to);

    relations.push_back(NodeRelation(this, &to, type, label)); //Add a new relation

    if(rels.size() == 1 && rels[0]->type == UNDEFINED) { //I had already an undefined relation to the destination node
	
	TRACE("Will replaced an old UNDEFINED relation by a better one!");

	//**** TODO !!! *****/

	//if (edge_p != NULL)
	//    edge_p->removeReferenceRelation(*this);

	//std::remove(relations.begin(), relations.end(), *(rels[0]));
    }

    TRACE("Added relation from " << id << " to " << to.getID());

    return relations.back();

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


void Node::updateKineticEnergy() {
    kinetic_energy = mass * speed.length2();
}


void Node::step(Graph& g, float dt){

    if(!stepDone) {

	/** Compute here the new position of the node **/

	// Algo from Wikipedia -- http://en.wikipedia.org/wiki/Force-based_layout

	TRACE("Stepping for node " << id);

	vec2f force = vec2f(0.0, 0.0);

	coulombForce = g.coulombRepulsionFor(*this);
	force +=coulombForce;

	hookeForce = g.hookeAttractionFor(*this);
	force += hookeForce;

	TRACE("** Total force applying: Fx=" << force.x << ", Fy= " << force.y);

	speed = (speed + force * dt) * damping;

	updateKineticEnergy();

	//Check we have enough energy to move :)
	if (kinetic_energy > MIN_KINETIC_ENERGY) {
	    pos += speed * dt;
	}

	TRACE("Node " << id << " now in pos=(" << pos.x << ", " << pos.y <<")");


	//TRACE("Step computed for " << id << ". Speed is " << speed.x << ", " << speed.y << " (energy: " << kinetic_energy << ").");
	stepDone = true;
    }
}

void Node::render(bool complete, bool debug){

#ifndef TEXT_ONLY

    if (debug) {
	vec4f col(1.0, 0.2, 0.2, 0.7);
	OroView::drawVector(hookeForce , pos, col);

	col = vec4f(0.2, 1.0, 0.2, 0.7);
	OroView::drawVector(coulombForce , pos, col);
    }

	renderer.renderAt(pos);
#endif

}

void Node::renderName(FXFont& font, bool debug){

#ifndef TEXT_ONLY
	renderer.renderName(pos, font);
#endif

}

void Node::setSelected(bool select) {

    if ((select && selected)||
	(!select && !selected)) return;

    selected = select;
    renderer.setSelected(select);

    if(select) charge *= 20;
    else charge /= 20;

}
