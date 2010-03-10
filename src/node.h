
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

#include "core/vectors.h"

#include "styles.h"
#include "node_renderer.h"
#include "node_relation.h"

class Graph;

class Node
{
    std::vector<NodeRelation> relations;

    bool selected;

    std::string id;

    void updateKineticEnergy();

public:

    Node(std::string id, const Node* neighbour = NULL);

    bool operator< (const Node& node) const;

    NodeRenderer renderer;

    float kinetic_energy;
    float charge;
    float mass;
    float damping;
    vec2f speed;
    vec2f pos;

    vec2f hookeForce;
    vec2f coulombForce;

    const std::string& getID() const;

    std::vector<NodeRelation>& getRelations();

    NodeRelation& addRelation(Node& to, const relation_type type, const std::string& label);
    /**
      Returns a vector of all the relations of *this that link to node.
      Returns an empty vector if no relation exist between *this and node.
      */
    std::vector<const NodeRelation*> getRelationTo(Node& node) const;

    /**
      executes one computation step to compute the position of the node according to other nodes.
      */
    void step(Graph& g, float dt);

     /**
      Renders the node. If called with rendering mode 'SIMPLE', goes in simple mode.

      In simple mode, no special effects are rendered. Useful for picking selected
      primitive in OpenGL GL_SELECT mode.
      */
    void render(rendering_mode mode, bool debug = false);

    void setColour(vec4f col);

    void setSelected(bool selected);
};

#endif // NODE_H
