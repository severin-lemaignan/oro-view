//#include <boost/test/minimal.hpp>

#include "macros.h"

#include "core/sdlapp.h"
#include "core/display.h"

//#include "graph.h"
#include "oroview.h"

using namespace std;

int main(int argc, char *argv[]) {
    int width  = 1024;
    int height = 768;
    bool fullscreen=false;
    bool multisample=false;
    vec3f background = vec3f(0.1, 0.1, 0.1);

    int video_framerate = 60;

    float start_position = 0.0;
    float stop_position  = 0.0;
    bool stop_on_idle = false;
    bool stop_at_end = false;

    std::string camera_mode = "overview";

    std::vector<std::string> arguments;

    SDLAppInit("ORO View", "oroview");

    SDLAppParseArgs(argc, argv, &width, &height, &fullscreen, &arguments);

#ifndef TEXT_ONLY

	// this causes corruption on some video drivers
    if(multisample) {
	display.multiSample(4);
    }

    //enable vsync
    display.enableVsync(true);

    try {

	display.init("ORO View", width, height, fullscreen);

    } catch(SDLInitException& exception) {

	throw OroViewException(string("SDL initialization failed ") + exception.what());
    }

    if(multisample) glEnable(GL_MULTISAMPLE_ARB);

#endif

    OroView* oroview = NULL;

    try {
	oroview = new OroView();

	 if(camera_mode == "track") {
	    oroview->setCameraMode(true);
	}

	oroview->setBackground(background);

	oroview->run();

    } catch(ResourceException& exception) {

	if (oroview != NULL) delete oroview;

	throw OroViewException(string("failed to load resource ") + exception.what());

    }

    if (oroview != NULL) delete oroview;

#ifndef TEXT_ONLY

    //free resources
    display.quit();

#endif

    return 0;

}


/****** UNIT TESTS *********/
//En attendant d'avoir deux cibles dans le Makefile, on peut activer
//l'unit testing en decommentant la premiere ligne de ce fichier, et
//en commentant l'autre main()
/*
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
*/
