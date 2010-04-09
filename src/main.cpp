/*
    Copyright (c) 2010 Séverin Lemaignan (slemaign@laas.fr)
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

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


#include "macros.h"

#include "core/sdlapp.h"
#include "core/display.h"

#include "oroview.h"

using namespace std;

void usage() {
	cout << "Usage: oro-view [OPTION...]" << endl << endl;
	cout << "  -h          display this message and exits" << endl;
	cout << "  -i file     load an input file" << endl;
	cout << "  -f          fullscreen mode" << endl;
	cout << "  -LxH        windowed mode, LxH pixels window" << endl;
	cout << endl;
	cout << "oro-view is an OpenGL-based visualization tool for RDF/OWL";
	cout << " ontologies" << endl;
	cout << "Séverin Lemaignan, LAAS-CNRS 2010, based on Gource by ";
	cout << "Andrew Caudwell" << endl << endl;
	cout << "Report bugs to openrobots@laas.fr" << endl;
}

int main(int argc, char *argv[]) {
	
	if (argc > 1 && !strcmp(argv[1],"-h")) {
		usage();
		exit(0);
	}

	string file_path  = "";
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

    SDLAppParseArgs(argc, argv, file_path, &width, &height, &fullscreen, &arguments);

	if (file_path != "")
		cout << "Using input file " << file_path << endl;
		
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
