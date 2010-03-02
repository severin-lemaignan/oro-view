#include "oroview_exceptions.h"

#include "node_relation.h"

#include "node.h"

NodeRelation::NodeRelation(Node* from, Node* to, const relation_type type, const std::string& label) :
	from(from),
	to(to),
	type(type),
	label(label){}

