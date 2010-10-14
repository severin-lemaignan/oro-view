 /*
 * ©LAAS-CNRS 2010
 *
 * contributor(s) : Séverin Lemaignan <severin.lemaignan@laas.fr>
 *
*/

#include <iostream>
#include <iterator>
#include <locale>

#include <boost/variant.hpp>

#include "oro_connector.h"

#include "oroview.h"
#include "node_relation.h"

#include "oro_exceptions.h"


#include "json/json.h"


using namespace std;
using namespace oro;

OntologyConnector::OntologyConnector() : sc("localhost", "6969")
{
    
    oro = Ontology::createWithConnector(sc);
}

const string OntologyConnector::get_edge_label(relation_type type, const string& original_label) {
    
    switch(type) {
        case SUBCLASS:
        case SUPERCLASS:
        case CLASS:
        case INSTANCE:
        case COMMENT:
            return "";
        
        default:
            return original_label;
    }
}

void OntologyConnector::walkThroughOntology(const string& from_node, int depth, OroView* graph) {
    
    if (depth == 0) return;
    
    //We need a collate object to compute hashes of literals
    locale loc;                 // the "C" locale
    const collate<char>& coll = use_facet<collate<char> >(loc);
    
    
    string details;
    string id;
    vec4f color;

    try {
        oro->getResourceDetails(from_node, details);
        cout << details << endl;
    }
    catch (ResourceNotFoundOntologyException& e) {
        cerr << "Node " + from_node + " not found in the ontology. Continuing." << endl;
        return;
    }
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( details, root );
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        cout  << "Failed to parse configuration\n"
                   << reader.getFormatedErrorMessages();
        return;
    }

    string name = root.get("name", "NO_NAME").asString();
    cout << "Analysing node : " << name << endl;

    string type = root.get("type", "NO_TYPE").asString();

    const Json::Value attributes = root["attributes"];
    for ( int index = 0; index < attributes.size(); ++index ) { // Iterates over the sequence elements.
        
        string oroType = attributes[index]["name"].asString();
        cout << endl << oroType << ":" << endl;
        
        Json::Value values = attributes[index]["values"];
        
        relation_type type = PROPERTY;
        
        if (oroType == "Parents") {type = SUBCLASS;}
        else if (oroType == "Children") {type = SUPERCLASS;}
        else if (oroType == "Instances") {type = CLASS;}
        else if (oroType == "Classes") {type = INSTANCE;}
        else if (oroType == "comment") {type = COMMENT;}
        
        for ( int index = 0; index < values.size(); ++index ) { // Iterates over the sequence elements.
            cout << "  - " << values[index]["name"].asString() << endl;

            if (values[index]["id"].asString() == "literal") { //build a hash for each literal based on "full name": current node + predicate + literal value
                string full_name = from_node + oroType + values[index]["name"].asString();
                ostringstream o;
                o << "literal_" << coll.hash(full_name.data(),full_name.data()+full_name.length());
                id = o.str();
            }
            else
                id = values[index]["id"].asString();

            graph->addNodeConnectedTo(
                    id,
                    values[index]["name"].asString(),
                    from_node, 
                    type, 
                    get_edge_label(type, attributes[index]["name"].asString()));
            
            walkThroughOntology(values[index]["name"].asString(),
                                depth - 1,
                                graph);
        }
    }

}

/*
int main(void) {
    set<Concept> result;
    string details;

    SocketConnector connector("localhost", "7000");
    Ontology *oro = Ontology::createWithConnector(connector);

    oro->getResourceDetails("baboon", details);

    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( details, root );
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        cout  << "Failed to parse configuration\n"
                   << reader.getFormatedErrorMessages();
        return 0;
    }

    string name = root.get("name", "NO_NAME").asString();
    cout << "Analysing node : " << name << endl;

    const Json::Value attributes = root["attributes"];
    for ( int index = 0; index < attributes.size(); ++index ) { // Iterates over the sequence elements.
        cout << endl << attributes[index]["name"].asString() << ":" << endl;
        Json::Value values = attributes[index]["values"];
       
        for ( int index = 0; index < values.size(); ++index ) { // Iterates over the sequence elements.
            cout << "  - " << values[index]["name"].asString() << endl;
        }
    }

    return 0;
}
*/
