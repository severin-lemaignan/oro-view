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

Graph* Graph::_instance = NULL;

Graph::Graph()
{
}

//Singleton access
Graph* Graph::getInstance(){
	if (_instance == NULL)
	    _instance = new Graph();
	return _instance;
}

Graph::~Graph() {
    delete _instance;
}

void Graph::step(float dt) {

    BOOST_FOREACH(NodeMap::value_type& n, nodes) {

	n.second.step(dt);
    }

    BOOST_FOREACH(Edge& e, edges) {
	e.step(dt);
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

void Graph::addEdge(NodeRelation& rel) {

    //Don't add an edge if the relation is between the same node.
    //It could be actually useful, but it provokes a segfault somewhere :-/
    if (rel.from == rel.to) {
	TRACE("Leaving immediately: from == to");
	return;
    }
    //The general idea is: several relations between nodes, but only ONE edge.
    //So we want to reuse existing edges.

    bool foundEdge = false;

    //Let's see if we need to create an edge for this relation
    // if it has already been created by the other node, just set the right references
    BOOST_FOREACH(const NodeRelation* r, rel.to->getRelationTo(*(rel.from))) {
	if (r->edge_p !=NULL) { //we found an edge!
	    rel.edge_p = r->edge_p;
	    rel.edge_p->addReferenceRelation(rel);
	    if (rel.edge_p == NULL) throw OroViewException("raaaah - A");
	    foundEdge = true;
	    return;
	}
    }

    if (!foundEdge) { //Check we don't already have ourself another relation with the node that may have an edge.
	BOOST_FOREACH(const NodeRelation* r, rel.from->getRelationTo(*(rel.to))) {
	    if (r->edge_p !=NULL) { //we found an edge!
		rel.edge_p = r->edge_p;
		rel.edge_p->addReferenceRelation(rel);
		foundEdge = true;
		return;
	    }
	}
    }

   //so now we are confident that there's no edge we can reuse. Let's create a new one.
    edges.push_back(Edge(rel));

    Edge& newEdge = edges.back();

    rel.edge_p = &newEdge;

    if (newEdge.countRelations() == 1 || !newEdge.hasOutboundConnectionFrom(rel.to)) { //bad! this edge doesn't have the reverse relation! :)
	rel.to->addRelation(*rel.from, UNDEFINED, "");
	TRACE("Added UNDEFINED back-relation from " << rel.to->getID() << " to " << rel.from->getID());
    }

    if (rel.edge_p == NULL) throw OroViewException("raaaah - C");
    return;
}

int Graph::nodesCount() {
    return nodes.size();
}

int Graph::edgesCount() {
    return edges.size();
}

