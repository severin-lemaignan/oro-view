
#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>

#include "oroview_exceptions.h"

#include "node.h"

class NodeRelation;
class Edge;

class Graph
{

private:
    /**
      a map of nodes. The key is the node ID.
     */
    typedef std::map<std::string, Node> NodeMap;
    NodeMap nodes;

    typedef std::vector<Edge> EdgeVector;
    EdgeVector edges;

    //We build a singleton
    static Graph* _instance;

protected:
    Graph();

public:
    ~Graph();

    static Graph* getInstance();

    void step();

    void render();

    void initializeNextStep();

    /**
      Returns a reference to a node by its id. Throws an exception if the node doesn't exist.
      */
    Node& getNode(const std::string& id);

    /**
      Adds a new node to the graph (if it doesn't exist yet) and returns a reference to the new node.
      */
    Node& addNode(const std::string& id);

    /**
      Adds a new edge to the graph (if it doesn't exist yet) between rel.from and rel.to
      Returns a reference to the new edge.

      It stores as well in the Edge object the reference to the relation.
      */
    Edge& addEdge(NodeRelation& rel);

    int nodesCount();
    int edgesCount();
};

#endif // GRAPH_H
