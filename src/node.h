
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

#include "core/sdlapp.h"

#include "macros.h"
#include "noderelation.h"
#include "node_renderer.h"

class Node
{
    std::vector<NodeRelation> relations;

    bool stepDone;
    bool renderingDone;

    std::string id;

    NodeRenderer renderer;

public:

    vec2f pos;

    Node(std::string id);

    bool operator< (const Node& node) const;

    const std::string& getID() const;

    std::vector<NodeRelation>& getRelations();

    void addRelation(Node& to, const relation_type type, const std::string& label);
    /**
      Returns a vector of all the relations of *this that link to node.
      Returns an empty vector if no relation exist between *this and node.
      */
    std::vector<const NodeRelation*> getRelationTo(Node& node) const;

    void initializeNextStep();

    /**
      executes one computation step to compute the position of the node according to other nodes.
      */
    void step();

    void render();
};

#endif // NODE_H
