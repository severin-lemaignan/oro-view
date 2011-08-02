 /*
 * ©LAAS-CNRS 2010
 *
 * contributor(s) : Séverin Lemaignan <severin.lemaignan@laas.fr>
 *
*/

#include <iostream>
#include <iterator>
#include <locale>

#include <boost/foreach.hpp>
#include <boost/variant.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include "oro_connector.h"

#include "oroview.h"
#include "node_relation.h"

#include "oro_exceptions.h"


#include "json/json.h"


using namespace std;
using namespace oro;
using namespace boost;



OntologyConnector::OntologyConnector() : sc("localhost", "6969")
{

    oro = Ontology::createWithConnector(sc);

    // Register the callback
    oro::Class("ActiveConcept").onNewInstance(*this);
}

const string OntologyConnector::getEdgeLabel(relation_type type, const string& original_label)
{
    switch(type) {
        case SUBCLASS:
        case SUPERCLASS:
        case CLASS:
        case INSTANCE:
            //return "is-a"; // display the 'is-a' label on every class->{subclass|instance} edges
        case COMMENT:
            return "";

        default:
            return original_label;
    }
}

void OntologyConnector::operator()(const OroEvent& evt)
{
    set<Concept> evt_content = boost::get<set<Concept> >(evt.content);

    cout << "New active concepts!" << endl;
    copy(evt_content.begin(), evt_content.end(), ostream_iterator<Concept>(cout, "\n"));

    lock_guard<mutex> l(active_concept_mutex);

    active_concepts_id.clear();

    BOOST_FOREACH(Concept c, evt_content) {
        string id = c.id();
        active_concepts_id.insert(id);
    }

}

const set<string> OntologyConnector::popActiveConceptsId()
{
    lock_guard<mutex> l(active_concept_mutex);

    set<string> res = active_concepts_id;
    active_concepts_id.clear();

    return res;
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

        if (oroType == "Parents") {type = SUPERCLASS;}
        else if (oroType == "Children") {type = SUBCLASS;}
        else if (oroType == "Instances") {type = INSTANCE;}
        else if (oroType == "Classes") {type = CLASS;}
        else if (oroType == "comment") {type = COMMENT;}

        for ( int index = 0; index < values.size(); ++index ) { // Iterates over the sequence elements.

            string to_label = values[index]["name"].asString();
            string to_id = values[index]["id"].asString();

            cout << "  - " << to_label << endl;

            if (to_id == "literal") { //build a hash for each literal based on "full name": current node + predicate + literal value
                string full_name = from_node + oroType + to_label;
                ostringstream o;
                o << "literal_" << coll.hash(full_name.data(),full_name.data()+full_name.length());
                to_id = o.str();
            }

            graph->addNodeConnectedTo(
                    to_id,
                    to_label,
                    from_node,
                    type,
                    getEdgeLabel(type, oroType));

            walkThroughOntology(to_id,
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
