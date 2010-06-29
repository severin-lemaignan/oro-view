 /*
 * ©LAAS-CNRS 2010
 *
 * contributor(s) : Séverin Lemaignan <severin.lemaignan@laas.fr>
 *
*/

#include <iostream>
#include <iterator>
#include <boost/variant.hpp>

#include "oro_connector.h"

#include "oroview.h"
#include "node_relation.h"

#include "liboro/oro_exceptions.h"


#include "json/json.h"


using namespace std;
using namespace oro;

OntologyConnector::OntologyConnector() : sc("localhost", "7000")
{
        oro = Ontology::createWithConnector(sc);
}
    
void OntologyConnector::walkThroughOntology(const string& from_node, int depth, OroView* graph) {
    
    if (depth == 0) return;
    
    string details;
    string id;
    vec4f color;

    try {
        oro->getResourceDetails(from_node, details);
        cout << details << endl;
    }
    catch (ResourceNotFoundOntologyException& e) {
        cerr << "Node " + from_node + " not found. Continuing." << endl;
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

    if (type == "class")
        color = CLASSES_COLOUR;
    else {
        if  (type == "instance")
            color = INSTANCES_COLOUR;
        else
            color = LITERAL_COLOUR;
    }

    try {
        graph->getNode(from_node).setColour(color);
    }
    catch(OroViewException& exception) {}

    const Json::Value attributes = root["attributes"];
    for ( int index = 0; index < attributes.size(); ++index ) { // Iterates over the sequence elements.
        cout << endl << attributes[index]["name"].asString() << ":" << endl;
        Json::Value values = attributes[index]["values"];
       
        for ( int index = 0; index < values.size(); ++index ) { // Iterates over the sequence elements.
            cout << "  - " << values[index]["name"].asString() << endl;
            if (values[index]["id"].asString() == "literal")
                id = values[index]["name"].asString();
            else
                id = values[index]["id"].asString();

            graph->addNodeConnectedTo(
                    id,
                    values[index]["name"].asString(),
                    from_node, 
                    SUBCLASS, 
                    attributes[index]["name"].asString());
            
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
