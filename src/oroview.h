#ifndef OROVIEW_H
#define OROVIEW_H

#include "core/display.h"
#include "core/sdlapp.h"
#include "core/frustum.h"
#include "core/fxfont.h"

#include "zoomcamera.h"

#include "oroview_exceptions.h"
#include "constants.h"

#include "graph.h"

class Node;

class OroView : public SDLApp {

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
    Node* selectedNode;
    void selectNode(Node* node);
    Bounds2D nodesBounds;

    bool track_users;
    void* selectedUser;

    Bounds2D usersBounds;

    //Mouse
    GLint mouse_hits;

    bool mousemoved;
    bool mouseclicked;
    bool mousedragged;

    float mouse_inactivity;

    vec2f mousepos;

    GLuint selectionDepth;

    //Background
    vec2f backgroundPos;
    bool backgroundSelected;
    vec3f background_colour;

    void selectBackground();

    //Loading screen
    bool draw_loading;
    void loadingScreen();

    /**
      When true, display in the application debug infos like framerate.
      */
    bool debug;

    bool paused;

    //Resources
    FXFont font, fontlarge, fontmedium;
    TextureResource* bloomtex;
    TextureResource* beamtex;

    //Drawing routines
    void drawBloom(Frustum &frustum, float dt);
    void drawBackground(float dt);
    void displayCoulombField();

    //Logic routines
    void mouseTrace(Frustum& frustum, float dt); //render the mouse and update hovered objects

    //Camera
    ZoomCamera camera;
    void updateCamera(float dt);
    void toggleCameraMode();
    void zoom(bool zoomin);

    //Testing
    /**
      Adds (amount) of new nodes to the graph, with (nb_rel) random connections to other nodes
      */
    void addRandomNodes(int amount, int nb_rel);


public:
    OroView();

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

};

#endif // OROVIEW_H
