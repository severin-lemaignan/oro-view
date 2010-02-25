#ifndef OROVIEW_H
#define OROVIEW_H

#include "core/display.h"
#include "core/sdlapp.h"
#include "core/frustum.h"
#include "core/fxfont.h"

#include "zoomcamera.h"

#include "oroview_exceptions.h"

static const std::string dateFormat("%A, %d %B, %Y %X");

class OroView : public SDLApp {
    time_t currtime;

    Uint32 draw_time;
    Uint32 logic_time;
    Uint32 trace_time;

    float time_scale;

    std::string displaydate;

    float runtime;
    int framecount;
    float max_tick_rate;

    int date_x_offset;

    ZoomCamera camera;

    bool track_users;
    void* selectedUser;

    bool backgroundSelected;

    vec3f background_colour;

    bool draw_loading;

    //Resources
    FXFont font, fontlarge, fontmedium;
    TextureResource* bloomtex;
    TextureResource* beamtex;

    //Drawing routines
    void drawBackground(float dt);

    void loadingScreen();

    //Various helpers
    void updateTime();

public:
    OroView();

    void setBackground(vec3f background);
    void setCameraMode(bool track_users);
    void toggleCameraMode();

    //Main routines
    void update(float t, float dt); //overrides SDLApp::update
    void logic(float t, float dt); //overrides SDLApp::logic
    void draw(float t, float dt); //overrides SDLApp::draw

};

#endif // OROVIEW_H
