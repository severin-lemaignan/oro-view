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

#include "oro.h"
#include "socket_connector.h"
#include "constants.h"

class OroView;

class OntologyConnector {
    
public:
    OntologyConnector();
    
    void walkThroughOntology(const std::string& from_node, int depth, OroView* graph);
    
private:
    oro::Ontology *oro;
    oro::SocketConnector sc;
    
    const std::string get_edge_label(relation_type type, const std::string& original_label);
};

#endif // ORO_CONNECTOR_H
