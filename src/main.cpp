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

#include <boost/program_options.hpp>

#include <fstream>
#include <json/json.h>

#include "macros.h"

#include "core/sdlapp.h"
#include "core/display.h"

#include "oroview.h"

using namespace std;
namespace po = boost::program_options;



int main(int argc, char *argv[]) {

    Json::Value config;

    int width;
    int height;
    bool fullscreen=false;
    bool multisample=false;

    int video_framerate = 60;

    po::positional_options_description p;
    p.add("configuration", 1);

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("fullscreen,f", "fullscreen")
            ("g", po::value<string>()->default_value("1024x768"), "window geometry (LxH)")
            ("configuration", po::value<string>(), "rendering configuration (JSON, optional)")
            ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                        .options(desc)
                        .positional(p)
                        .run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << "memory-view -- A viewer for associative memory networks\n\n" << desc << "\n";
        cout << "memory-view is an OpenGL-based visualization tool for" << endl;
        cout << "associative memory networks." << endl << endl;
        cout << "Séverin Lemaignan, Plymouth University 2016, " << endl;
        cout << "initially based on Gource by Andrew Caudwell" << endl;
        cout << "Report bugs to: " << endl;
        cout << "https://www.github.com/severin-lemaignan/memory-view/issues" << endl;

        return 1;
    }

    if (vm.count("fullscreen")) {
        fullscreen = true;
    }

    if (vm.count("geometry")) {
        
        //get video mode
        auto argstr = vm["geometry"].as<string>();
        size_t x = argstr.rfind("x");

        if(x != std::string::npos) {
            std::string widthstr  = argstr.substr(0, x);
            std::string heightstr = argstr.substr(x+1);

            int w = atoi(widthstr.c_str());
            int h = atoi(heightstr.c_str());

            if(width!=0 && height!=0) {
                debugLog("w=%d, h=%d\n",width,height);

                width = w;
                height = h;
            }
        }
    }


    float start_position = 0.0;
    float stop_position  = 0.0;
    bool stop_on_idle = false;
    bool stop_at_end = false;

    std::string camera_mode = "overview";

    if (vm.count("configuration")) {
        auto conf = vm["configuration"].as<string>();
        cout << "Using configuration file " << conf << endl;
        Json::Reader reader;
        ifstream conf_file(conf, ifstream::binary);
        bool parsingOk = reader.parse(conf_file, config);
        if (!parsingOk) {
            cerr << "Error while parsing the configuration file!\n"
                 << reader.getFormattedErrorMessages();
            exit(1);
        }
    }

    SDLAppInit("Memory View", "memory-view");

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
        oroview = new OroView(config);

        if(camera_mode == "track") {
            oroview->setCameraMode(true);
        }

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
