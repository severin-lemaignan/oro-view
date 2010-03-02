#ifndef NODERELATION_H
#define NODERELATION_H

#include <string>
#include "macros.h"

#include "constants.h"

#include "edge.h"

class Node;

class NodeRelation {

	//The edge that represents this relation
	Edge* edge_p;
	Edge* init_edge_p;


public:
	NodeRelation(Node* from, Node* to, const relation_type type, const std::string& label);

	Node* from;
	Node* to;
	relation_type type;
	std::string label;

	Edge& getEdge() const;
	void setEdge(Edge& edge);
};

#endif // NODERELATION_H
