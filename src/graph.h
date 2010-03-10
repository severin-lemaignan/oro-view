
#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>

#include "oroview_exceptions.h"

#include "node.h"
#include "edge.h"
#include "node_relation.h"


class Graph
{
public:
    /**
      a map of nodes. The key is the node ID.
     */
    typedef std::map<int, Node> NodeMap;
    
private:
    
    NodeMap nodes;

    typedef std::vector<Edge> EdgeVector;
    EdgeVector edges;

public:
    Graph();

    void step(float dt);

    /**
      Renders the graph. If called with argument 'false', goes in simple mode.

      In simple mode, neither edges or special effects are rendered. Useful for picking selected
      primitive in OpenGL GL_SELECT mode.
      */
    void render(rendering_mode mode, bool debug = false);

    /**
      Returns an immutable reference to the list of nodes.
      */
    const NodeMap& getNodes() const;

    /**
      Returns a reference to a node by its id. Throws an exception if the node doesn't exist.
      */
    Node& getNode(const std::string& id);

    const Node& getConstNode(const std::string& id) const;

    /**
      Returns a reference to a node by its tagid, ie the hash value of its ID. Return a NULL pointer
      if the node doesn't exists.
      */
    Node* getNodeByTagID(int tagid);

    /**
      Returns a random node.
      */
    Node& getRandomNode();

    /**
      Adds a new node to the graph (if it doesn't exist yet) and returns a reference to the new node.
      */
    Node& addNode(const std::string& id, const Node* neighbour = NULL);

    /**
      Adds a new edge to the graph (if it doesn't exist yet) between rel.from and rel.to
      Returns a reference to the new edge.

      It stores as well in the Edge object the reference to the relation.
      */
    void addEdge(Node& from, Node& to, const relation_type type, const std::string& label);

    std::vector<const Edge*> getEdgesFor(const Node& node) const;
    std::vector<Edge*> getEdgesBetween(const Node& node1, const Node& node2);

    int nodesCount();
    int edgesCount();

    vec2f coulombRepulsionFor(const Node& node) const;
    vec2f coulombRepulsionAt(const vec2f& pos) const;

    vec2f hookeAttractionFor(const Node& node) const;

    vec2f project(float force, const vec2f& d) const;

};

#endif // GRAPH_H
