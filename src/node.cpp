#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>

#include "node.h"
#include "graph.h"

using namespace std;
using namespace boost;

Node::Node(string id) : id(id), renderer(NodeRenderer(hash_value(id)))
{
    pos = vec2f(0.0, 0.0);



    initializeNextStep();
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

void Node::initializeNextStep(){
    stepDone = false;
    renderingDone = false;
}

void Node::step(float dt){

    if(!stepDone) {

	vec2f dpos = vec2f(5.0 * (float)rand()/RAND_MAX - 2.5 , 5.0 * (float)rand()/RAND_MAX - 2.5);

	pos += dpos;

	TRACE("Step computed for " << id << ". Now at (" << pos.x << ", " << pos.y << ").");
	stepDone = true;
    }
}

void Node::render(){

    if (!renderingDone) {
#ifndef TEXT_ONLY
	renderer.renderAt(pos);
#endif
	TRACE("Node " << id << " rendered.");
	renderingDone = true;
    }
}
