#include <string>

#include "oroview.h"

using namespace std;

OroView::OroView()
{
    time_scale = 1.0f;
    runtime = 0.0f;
    framecount = 0;

    //min phsyics rate 60fps (ie maximum allowed delta 1.0/60)
    max_tick_rate = 1.0 / 60.0;

    currtime = time(NULL);


    camera = ZoomCamera(vec3f(0,0, -300), vec3f(0.0, 0.0, 0.0), 250.0, 5000.0);

    track_users = false;
    selectedUser = NULL;

    backgroundSelected = false;

    background_colour = vec3f(0.0, 0.0, 0.0);

    draw_loading = true;

    fontlarge = fontmanager.grab("FreeSans.ttf", 42);
    fontlarge.dropShadow(true);
    fontlarge.roundCoordinates(true);

    fontmedium = fontmanager.grab("FreeSans.ttf", 16);
    fontmedium.dropShadow(true);
    fontmedium.roundCoordinates(false);

    font = fontmanager.grab("FreeSans.ttf", 14);
    font.dropShadow(true);
    font.roundCoordinates(true);

    bloomtex = texturemanager.grab("bloom.tga");
    beamtex  = texturemanager.grab("beam.png");
}

void OroView::loadingScreen() {
    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    string loading_message("Initializing ORO View...");
    int width = font.getWidth(loading_message);

    font.print(display.width/2 - width/2, display.height/2 - 10, "%s", loading_message.c_str());
}

void OroView::setBackground(vec3f background) {
    background_colour = background;
}

void OroView::setCameraMode(bool track_users) {
    this->track_users = track_users;
    if(selectedUser!=NULL) camera.lockOn(track_users);
    backgroundSelected=false;
}

void OroView::toggleCameraMode() {
    setCameraMode(!track_users);
}

void OroView::update(float t, float dt) {

    dt = min(dt, max_tick_rate);

    dt *= time_scale;

    //have to manage runtime internally as we're messing with dt
    runtime += dt;

    logic_time = SDL_GetTicks();

    logic(runtime, dt);

    logic_time = SDL_GetTicks() - logic_time;

    draw_time = SDL_GetTicks();

    draw(runtime, dt);

    framecount++;
}

void OroView::logic(float t, float dt) {
     if(draw_loading && logic_time > 1000) draw_loading = false;
}

void OroView::drawBackground(float dt) {
    display.setClearColour(background_colour);
    display.clear();
}

void OroView::draw(float t, float dt) {

    display.mode2D();

    drawBackground(dt);

    if(draw_loading) {
	loadingScreen();
	return;
    }

    Frustum frustum(camera);

    trace_time = SDL_GetTicks();

//    mousetrace(frustum,dt);

    trace_time = SDL_GetTicks() - trace_time;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    camera.focus();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    //draw tree
//    drawTree(frustum, dt);
//
//    glColor4f(1.0, 1.0, 0.0, 1.0);
//    for(map<string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
//        trace_debug ? it->second->drawSimple(dt) : it->second->draw(dt);
//    }
//
//    glEnable(GL_TEXTURE_2D);
//    glEnable(GL_BLEND);
//
//    //draw bloom
//    drawBloom(frustum, dt);
//
//    root->drawNames(font,frustum);
//
//    if(!(gGourceHideUsernames || gGourceHideUsers)) {
//        for(map<string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
//            it->second->drawName();
//        }
//    }
//
//    //draw selected item names again so they are over the top
//    if(selectedUser !=0) selectedUser->drawName();
//
//    if(selectedNode !=0) {
//        vec2f dirpos = selectedNode->getDir()->getPos();
//
//        glPushMatrix();
//            glTranslatef(dirpos.x, dirpos.y, 0.0);
//            selectedNode->drawName();
//        glPopMatrix();
//    }

    //debug
    if(false) {
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

//	track_users ? user_bounds.draw() : dir_bounds.draw();
    }

    //gGourceQuadTreeDebug
    if(false) {
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glLineWidth(1.0);
//	dirNodeTree->outline();

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

//	userTree->outline();
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    vec3f campos = camera.getPos();

    fontmedium.draw(display.width/2 - date_x_offset, 20, displaydate);

    glDisable(GL_TEXTURE_2D);


//    mousemoved=false;
//    mouseclicked=false;
}


void OroView::updateTime() {
    //display date
    char datestr[256];
    char timestr[256];
    struct tm* timeinfo = localtime ( &currtime );

    strftime(datestr, 256, dateFormat.c_str(), timeinfo);
    displaydate = datestr;

    //avoid wobbling by only moving font if change is sufficient
    int date_offset = (int) fontmedium.getWidth(displaydate) * 0.5;
    if(abs(date_x_offset - date_offset) > 5) date_x_offset = date_offset;
}
