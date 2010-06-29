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

#include "liboro/socket_connector.h"

#include "json/json.h"


using namespace std;
using namespace oro;

OntologyConnector::OntologyConnector() {
        SocketConnector connector("localhost", "7000");
        oro = Ontology::createWithConnector(connector);
}

void OntologyConnector::walkThroughOntology(const string& from_node, int depth, OroView* graph) {
        graph->addNodeConnectedTo("Toto", "Animal", SUBCLASS, "subclass");
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
