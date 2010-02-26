
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

#include "core/vectors.h"

#include "constants.h"
#include "node_renderer.h"

class NodeRelation;

class Node
{
    std::vector<NodeRelation> relations;

    bool stepDone;
    bool renderingDone;

    std::string id;

    vec2f speed;

public:

    Node(std::string id);

    bool operator< (const Node& node) const;

    NodeRenderer renderer;

    vec2f pos;

    const std::string& getID() const;

    std::vector<NodeRelation>& getRelations();

    void addRelation(Node& to, const relation_type type, const std::string& label);
    /**
      Returns a vector of all the relations of *this that link to node.
      Returns an empty vector if no relation exist between *this and node.
      */
    std::vector<const NodeRelation*> getRelationTo(Node& node) const;

    void resetRenderers();

    /**
      executes one computation step to compute the position of the node according to other nodes.
      */
    void step(float dt);

     /**
      Renders the node. If called with argument 'false', goes in simple mode.

      In simple mode, no special effects are rendered. Useful for picking selected
      primitive in OpenGL GL_SELECT mode.
      */
    void render(bool complete = true);
};

#endif // NODE_H
