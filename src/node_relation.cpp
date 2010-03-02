#include "oroview_exceptions.h"

#include "node_relation.h"

#include "node.h"

NodeRelation::NodeRelation(Node* from, Node* to, const relation_type type, const std::string& label) :
	from(from),
	to(to),
	type(type),
	label(label),
	edge_p(NULL),
	init_edge_p(NULL){}

Edge& NodeRelation::getEdge() const {
    if (edge_p == NULL) throw OroViewException("Edge not set for this relation!");
    if (edge_p != init_edge_p) throw OroViewException("Someone touched my edge!");
    return *edge_p;
}

void NodeRelation::setEdge(Edge& edge) {
    if (init_edge_p == NULL) init_edge_p = &edge;
    edge_p = &edge;
    TRACE("Edge set from " << from->getID() << " to " << to->getID() << " (" << &edge << ")");
    //edge.addReferenceRelation(*this);
}
