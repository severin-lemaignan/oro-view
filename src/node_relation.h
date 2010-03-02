#ifndef NODERELATION_H
#define NODERELATION_H

#include <string>
#include "macros.h"

#include "constants.h"

#include "edge.h"

class Node;

class NodeRelation {


public:
	NodeRelation(Node* from, Node* to, const relation_type type, const std::string& label);

	Node* from;
	Node* to;
	relation_type type;
	std::string label;

};

#endif // NODERELATION_H
