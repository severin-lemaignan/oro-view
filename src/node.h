/*
    Copyright (c) 2010 Séverin Lemaignan (slemaign@laas.fr)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

    std::string id;

    void updateKineticEnergy();

public:

    Node(const std::string& id, const std::string& label, const Node* neighbour = NULL, node_type type = CLASS_NODE);

    bool operator< (const Node& node) const;

    NodeRenderer renderer;

    float kinetic_energy;
    float charge;
    float mass;
    float damping;
    vec2f speed;
    vec2f pos;

    bool selected;
     /** The (minimum) amount of nodes that link me to the selected node.
       If no node is selected, -1
    **/
    int distance_to_selected;
    bool distance_to_selected_updated;

    vec2f hookeForce;
    vec2f coulombForce;

    const std::string& getID() const;

    /**
      Returns a vector of all nodes connected to myself.
      */
    std::vector<Node*> getConnectedNodes();

    bool isConnectedTo(Node* node);

    /**
      Returns a vector of all relations this node has with other nodes.
      */
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
    void render(rendering_mode mode, OroView& env, bool debug = false);

    void setColour(vec4f col);

    /** 'Activates' the node by briefly changing its color, and fading back to
      the original hue.
      */
    void tickle();
    void setSelected(bool selected);
};

#endif // NODE_H
