#include <boost/test/minimal.hpp>

#include "macros.h"

#include "core/sdlapp.h"

#include "graph.h"

using namespace std;

void run() {
    TRACE("*** Entering main loop ***");
    while(true) {
	g.initializeNextStep();

	SDL_Delay(200);

	TRACE("\t - Stepping:");
	g.step();

	TRACE("\t - Now displaying:");
	g.render();

	TRACE("\t - End of loop!");
    }
}

int test_main(int argc, char *argv[]) {

    TRACE("*** Initialization ***");

    Graph& g = *(Graph::getInstance());


    Node& a = g.addNode("node1");
    Node& b = g.addNode("node2");

    TRACE("\t - Graph created and populated");

    TRACE("*** Starting tests ***");

    b.addRelation(a, OBJ_PROPERTY, "loves");

    BOOST_REQUIRE(!b.getRelationTo(a).empty());
    BOOST_REQUIRE((b.getRelationTo(a))[0]->label == "loves");

    BOOST_REQUIRE(g.edgesCount() == 1);
    BOOST_REQUIRE(g.nodesCount() == 2);

    g.addNode("node1"); //already exist

    BOOST_REQUIRE(g.nodesCount() == 2);

    a.addRelation(b, OBJ_PROPERTY, "hates");

    BOOST_REQUIRE(g.edgesCount() == 1); //the edge already exists. Should be reused.

    exit(0);

}
