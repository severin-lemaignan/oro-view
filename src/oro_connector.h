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

#ifndef ORO_CONNECTOR_H
#define ORO_CONNECTOR_H

#include <set>
#include <string>

#include <boost/thread/mutex.hpp>

#include <liboro/oro.h>
#include <liboro/socket_connector.h>

#include "constants.h"
#include "graph.h"

class OroView;

class OntologyConnector : public oro::OroEventObserver {

public:
    OntologyConnector(const std::string& host, const std::string& port, bool only_labelled_nodes = false);

    /**
      Adds a node to the graph, querying the ontology for its type and label.

      @return true if the node has been added, false if not (for instance, if the
      node has no label and only_labelled_node is true.
    */
    bool addNode(const std::string& id, Graph& g);
    void walkThroughOntology(const std::string& from_node, int depth, OroView* graph);

    const std::set<std::string> popActiveConceptsId();

    // Callback for oro events
    void operator()(const oro::OroEvent& evt);
private:

    bool only_labelled_nodes;

    std::set<std::string> active_concepts_id;
    oro::Ontology *oro;
    oro::SocketConnector sc;

    mutable boost::mutex active_concept_mutex;

    const std::string getEdgeLabel(relation_type type, const std::string& original_label);
};

#endif // ORO_CONNECTOR_H
