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

#ifndef OROVIEW_H
#define OROVIEW_H

#include <sstream>
#include <json/json.h>

#include "core/display.h"
#include "core/sdlapp.h"
#include "core/frustum.h"
#include "core/fxfont.h"

#include "zoomcamera.h"

#include "oroview_exceptions.h"
#include "constants.h"

#include "graph.h"

#include "oro_connector.h"

class Node;

class OroView : public SDLApp {

    //General configuration
    Json::Value config;

    //Graph
    Graph g;

    //Time
    time_t currtime;

    Uint32 draw_time;
    Uint32 logic_time;
    Uint32 trace_time;

    float idle_time;

    float time_scale;

    std::string displaydate;

    float runtime;
    int framecount;
    float max_tick_rate;

    int date_x_offset;

    void updateTime();

    //Nodes & users

    Node* hoverNode;
    void selectNode(Node* node);
    void addSelectedNode(Node* node);
    Bounds2D nodesBounds;
    bool display_node_infos;

    bool track_users;
    void* selectedUser;

    Bounds2D usersBounds;

    //Mouse
    GLint mouse_hits;

    bool mousemoved;
    bool mouseleftclicked;
    bool mouserightclicked;
    bool mousedragged;

    float mouse_inactivity;

    vec2f mousepos;

    GLuint selectionDepth;

    //Background
    vec2f backgroundPos;
    bool backgroundSelected;
    vec3f background_colour;

    void selectBackground();

    //Footer content maps a text to write to it's X coordinate.
    std::map<std::string, int> footer_content;
    void queueInFooter(const std::string& text);
    void queueNodeInFooter(const std::string& id);
    void drawFooter();

    //Loading screen
    bool draw_loading;
    void loadingScreen();

    /**
     * When true, display in the application debug infos like framerate.
     *
     * Normally, activated by pushing on D during runtime
     */
    bool debug;

    /**
     * When true, display advanced debug infos like force vectors
     *
     * Normally, activated by pushing two times on D during runtime
     */
    bool advanced_debug;

    bool paused;

    //Resources
    TextureResource* bloomtex;
    TextureResource* beamtex;

    //Connection to the ontology
    OntologyConnector oro;

    //Drawing routines
    void drawBloom(Frustum &frustum, float dt);
    void drawBackground(float dt);
    void displayCoulombField();

    //Logic routines
    void mouseTrace(Frustum& frustum, float dt); //render the mouse and update hovered objects

    //Camera

    void updateCamera(float dt);
    void zoom(bool zoomin);

    //Testing
    /**
      Adds (amount) of new nodes to the graph, with (nb_rel) random connections to other nodes
      */
    void addRandomNodes(int amount, int nb_rel);

    void updateCurrentNode();


    void stylesSetup(const Json::Value& config);
    void physicsSetup(const Json::Value& config);
    vec4f convertRGBA2Float(const Json::Value& color);

    // If false, do not display shadows
    bool display_shadows;
    // If false, do not display labels of nodes
    bool display_labels;
    // If false, do not display footer with active nodes
    bool display_footer;
    // If true, only display nodes that have a real label (ie, not only a node id)
    bool only_labelled_nodes;

public:
    OroView(const Json::Value& config);

    //Public resources
    FXFont font, fontlarge, fontmedium;

    // Textual version of the graph, in dot format.
    // Filled when calling render on node and/or edge in GRAPHVIZ mode
    std::stringstream graphvizGraph;

    //Public camera
    ZoomCamera camera;

    //Initialisation
    void init(); //overrides SDLApp::init

    //Events
    void keyPress(SDL_KeyboardEvent *e); //overrides SDLApp::keyPress
    void mouseClick(SDL_MouseButtonEvent *e); //overrides SDLApp::mouseClick
    void mouseMove(SDL_MouseMotionEvent *e); //overrides SDLApp::mouseMove

    //Main routines
    void update(float t, float dt); //overrides SDLApp::update
    void logic(float t, float dt); //overrides SDLApp::logic
    void draw(float t, float dt); //overrides SDLApp::draw

    //Camera
    void setCameraMode(bool track_users);

    //Background
    void setBackground(vec3f background);

    static void drawVector(vec2f vec, vec2f pos, vec4f col);


    /** Add node.

      @return true if the node has been added, false else (can be false for nodes
      without label is only_labelled_node is true)
      */
    bool addNodeConnectedTo(const std::string& id,
                            const std::string& node_label,
                            const std::string& to,
                            relation_type type,
                            const std::string& edge_label);

    void addAlias(const std::string& alias, const std::string& id);
    Node& getNode(const std::string& id);
};

#endif // OROVIEW_H
