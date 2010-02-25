#ifndef NODERELATION_H
#define NODERELATION_H

#include <string>

#include "edge.h"

class Node;

enum relation_type {SUBCLASS, SUPERCLASS, INSTANCE, OBJ_PROPERTY, DATA_PROPERTY};

struct NodeRelation {
	Node* from;
	Node* to;
	relation_type type;
	std::string label;

	//The edge that represents this relation
	Edge* edge_p;

	NodeRelation(Node* from, Node* to, const relation_type type, const std::string& label) :
								from(from),
								to(to),
								type(type),
								label(label),
								edge_p(NULL) {}
};

#endif // NODERELATION_H
