#include <boost/foreach.hpp>

#include "node.h"
#include "graph.h"

using namespace std;

Node::Node() {
    throw OroViewException("Default Node constructor called! shouldn't occur! Bug somewhere! (probably access to nodes[] on an element that doesn't exist.");
}
Node::Node(string id) : id(id)
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

void Node::step(){

    if(!stepDone) {
	TRACE("Step computed for " << id);
	stepDone = true;
    }
}

void Node::render(){

    if (!renderingDone) {
	TRACE("Node " << id << " rendered.");
	renderingDone = true;
    }
}

void Node::renderEdges(){
    TRACE("Doing nothing yet to render edges");
}
