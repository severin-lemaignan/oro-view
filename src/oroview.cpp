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

#include <string>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <json/json.h>

#include "oroview.h"

#include "macros.h"
#include "styles.h"

#include "node_relation.h"

#include "graph.h"

using namespace std;

OroView::OroView(const Json::Value& config):
    config(config),
    display_shadows(config.get("shadows", "true").asBool()),
    display_labels(config.get("display_labels", "true").asBool()),
    display_footer(config.get("display_footer", "true").asBool()),
    only_labelled_nodes(config.get("only_labelled_nodes", "false").asBool()),
    oro(config.get("oro_host", "localhost").asString(),
        config.get("oro_port", "6969").asString(),
        only_labelled_nodes)
{


    time_scale = 1.0f;
    runtime = 0.0f;
    framecount = 0;

    //min phsyics rate 60fps (ie maximum allowed delta 1.0/60)
    max_tick_rate = 1.0 / 60.0;

    currtime = time(NULL);

    srand(currtime);

    //Nodes & users

    hoverNode = NULL;

    track_users = false;
    selectedUser = NULL;

    //Mouse

    mousemoved = false;
    mouseleftclicked = false;
    mouserightclicked = false;
    mousedragged = false;

    backgroundSelected = false;


    draw_loading = true;
    display_node_infos = true;
    debug = false;
    advanced_debug = false;
    paused = false;

    fontlarge = fontmanager.grab("Aller_Bd.ttf", LARGE_FONT_SIZE);
    fontlarge.dropShadow(true);
    fontlarge.roundCoordinates(true);

    fontmedium = fontmanager.grab("Aller_Lt.ttf", MEDIUM_FONT_SIZE);
    fontmedium.dropShadow(true);
    fontmedium.roundCoordinates(false);

    font = fontmanager.grab("Aller_Lt.ttf", BASE_FONT_SIZE);
    font.dropShadow(true);
    font.roundCoordinates(true);

    camera = ZoomCamera(vec3f(0,0, -300), vec3f(0.0, 0.0, 0.0), 250.0, 5000.0);

#ifndef TEXT_ONLY
    bloomtex = texturemanager.grab("bloom.tga");
    beamtex  = texturemanager.grab("beam.png");
#endif



    stylesSetup(config);
    physicsSetup(config);

    background_colour = BACKGROUND_COLOUR.truncate();
}

void OroView::stylesSetup(const Json::Value& config) {

    Json::Value colors = config["colours"];

    if (colors == Json::nullValue) return; // Uses defaults, as specified in styles.h

    cout << "Setting customs colors from config file." << endl;
    if (colors["selected"] != Json::nullValue) {
        SELECTED_COLOUR = convertRGBA2Float(colors["selected"]);
    }

    if (colors["hovered"] != Json::nullValue)
        HOVERED_COLOUR = convertRGBA2Float(colors["hovered"]);

    if (colors["classes"] != Json::nullValue)
        CLASSES_COLOUR = convertRGBA2Float(colors["classes"]);

    if (colors["instances"] != Json::nullValue)
        INSTANCES_COLOUR = convertRGBA2Float(colors["instances"]);

    if (colors["literals"] != Json::nullValue)
        LITERALS_COLOUR = convertRGBA2Float(colors["literals"]);

    if (colors["background"] != Json::nullValue){
        BACKGROUND_COLOUR = convertRGBA2Float(colors["background"]);
    }


}

void OroView::physicsSetup(const Json::Value& config) {

    Json::Value physics = config["physics"];

    if (physics == Json::nullValue) return; // Uses defaults, as specified in constants.h

    cout << "Setting customs physics parameters from config file." << endl;
    if (physics["mass"] != Json::nullValue) {
        INITIAL_MASS = physics["mass"].asDouble();
    }
    if (physics["damping"] != Json::nullValue) {
        INITIAL_DAMPING = physics["damping"].asDouble();
    }
    if (physics["repulsion"] != Json::nullValue) {
        COULOMB_CONSTANT = physics["repulsion"].asDouble();
    }
    if (physics["maxspeed"] != Json::nullValue) {
        MAX_SPEED = physics["maxspeed"].asDouble();
    }


}

vec4f OroView::convertRGBA2Float(const Json::Value& color) {
    return vec4f(color[0u].asInt()/255.0,
                 color[1u].asInt()/255.0,
                 color[2u].asInt()/255.0,
                 color[3u].asInt()/255.0);
}

/** Initialization */
void OroView::init(){

    TRACE("*** Initialization ***");

    string root = config.get("initial_concept", ROOT_CONCEPT).asString();

    oro.addNode(root, g);
    TRACE("Starting with concept " << root);

    oro.walkThroughOntology(root, 2, this);

    TRACE("*** Graph created and populated ***");
    TRACE("*** STARTING MAIN LOOP ***");
}

/** Events */
void OroView::keyPress(SDL_KeyboardEvent *e) {
    if (e->type == SDL_KEYUP) return;

    if (e->type == SDL_KEYDOWN) {
        if (e->keysym.sym == SDLK_ESCAPE) {
            appFinished=true;
        }

        if (e->keysym.sym == SDLK_d) {
            if (debug)
                if (advanced_debug) {
                    debug = false;
                    advanced_debug = false;
                }
                else advanced_debug = true;
            else debug = true;
        }
        //        if (e->keysym.sym == SDLK_w) {
        //            gGourceQuadTreeDebug = !gGourceQuadTreeDebug;
        //        }

        if (e->keysym.sym == SDLK_SPACE) {
            //addRandomNodes(2, 2);
            updateCurrentNode();
        }

        if (e->keysym.sym == SDLK_t) {
            g.getRandomNode().tickle();
        }

        if (e->keysym.sym == SDLK_p) {
            paused = !paused;
        }

        if(e->keysym.sym == SDLK_UP) {
            zoom(true);
        }

        if(e->keysym.sym == SDLK_DOWN) {
            zoom(false);
        }

        if(e->keysym.sym == SDLK_s) {
            g.saveToGraphViz(*this);
        }
    }
}

void OroView::mouseClick(SDL_MouseButtonEvent *e) {

    if(e->type == SDL_MOUSEBUTTONUP) {

        if(e->button == SDL_BUTTON_LEFT) {

            mouse_inactivity=0.0;
            SDL_ShowCursor(true);

            SDL_WM_GrabInput(SDL_GRAB_OFF);

            //stop dragging mouse, return the mouse to where
            //the user started dragging.
            if(mousedragged) {
                SDL_WarpMouse(mousepos.x, mousepos.y);
                mousedragged=false;
            }
        }
    }

    if(e->type != SDL_MOUSEBUTTONDOWN) return;

    //wheel up
    if(e->button == SDL_BUTTON_WHEELUP) {
        zoom(true);
        return;
    }

    //wheel down
    if(e->button == SDL_BUTTON_WHEELDOWN) {
        zoom(false);
        return;
    }

    if(e->button == SDL_BUTTON_RIGHT) {
        mousepos = vec2f(e->x, e->y);
        mouserightclicked=true;

        return;
    }

    if(e->button == SDL_BUTTON_LEFT) {

        mousepos = vec2f(e->x, e->y);
        mouseleftclicked=true;
    }

}

void OroView::mouseMove(SDL_MouseMotionEvent *e) {

    mousepos = vec2f(e->x, e->y);

    Node* selectedNode = g.getSelected();

    //move camera in direction the user dragged the mouse
    if(mousedragged) {
        if (selectedNode != NULL) {
            selectedNode->pos += vec2f( e->xrel, e->yrel )/2;
        }
        else backgroundPos += vec2f( e->xrel, e->yrel );

        return;
    }

    mouse_inactivity = 0.0;
    SDL_ShowCursor(true);


    mousemoved=true;
}

/** main update function */
void OroView::update(float t, float dt) {

    SDL_Delay(20); //N'allons pas trop vite au début...

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

/** App logic */
void OroView::logic(float t, float dt) {
    if(draw_loading && logic_time > 1000) draw_loading = false;

    //still want to update camera while paused
    if(paused) {
        updateCamera(dt);
        return;
    }

    //check if mouse has been inactive for 3 seconds
    //and if so hide it.
    if(!mouseleftclicked && !mouserightclicked && mouse_inactivity<3.0) {
        mouse_inactivity += dt;

        if(mouse_inactivity>=3.0) {
            SDL_ShowCursor(false);
        }
    }

    BOOST_FOREACH(string id, oro.popActiveConceptsId()) {
        try {
            g.getNode(id).tickle();
            queueNodeInFooter(id);
        }
        catch(OroViewException& exception) {
            cout << "One of the active concept do not exist yet: " << id << ". Creating it." << endl;
            oro.addNode(id, g);
            g.getNode(id).tickle();
            queueNodeInFooter(id);
            oro.walkThroughOntology(id, 1, this);

        }
    }

    g.step(dt);

    updateCamera(dt);
}

void OroView::mouseTrace(Frustum& frustum, float dt) {

    GLuint	buffer[512];
    GLint	viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(512, buffer);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    /** Start the selection of primitives touched by the mouse **/
    (void) glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Restrict the detection to a zone around of the mouse
    gluPickMatrix((GLdouble) mousepos.x, (GLdouble) (viewport[3]-mousepos.y), 1.0f, 1.0f, viewport);
    gluPerspective(90.0f, (GLfloat)display.width/(GLfloat)display.height, 0.1f, camera.getZFar());
    camera.look();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
    //	it->second->drawSimple(dt);
    //    }

    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    g.render(SIMPLE, *this);

    glMatrixMode(GL_MODELVIEW);

    //going back to the GL_RENDER mode, returning the number of hits from hte GL_SELECT mode
    mouse_hits = glRenderMode(GL_RENDER);
    /** End of selection **/

    Node* nodeSelection = NULL;
    //    RUser* userSelection = 0;

    if (mouse_hits > 0) {

        int choice   = buffer[3];
        GLuint depth = buffer[1];

        for (int loop = 1; loop < mouse_hits; loop++) {
            if (buffer[loop*4+1] < depth) {
                choice = buffer[loop*4+3];
                depth  = buffer[loop*4+1];
            }
        }

        if(choice != 0) {
            selectionDepth = depth;
            nodeSelection = g.getNodeByTagID(choice);

            //	    else if((usertest = tagusermap.find(choice)) != tagusermap.end()) {
            //		userSelection = usertest->second;
            //	    }
        }
    }

    glDisable(GL_DEPTH_TEST);

    // is over a file
    if(nodeSelection != NULL) {
        //	// un hover a user
        //	if(hoverUser != 0) {
        //	    hoverUser->setMouseOver(false);
        //	    hoverUser = 0;
        //	}

        if(nodeSelection != hoverNode) {
            //deselect previous selection
            if(hoverNode !=NULL) hoverNode->renderer.setMouseOver(false);

            //select new
            nodeSelection->renderer.setMouseOver(true);
            hoverNode = nodeSelection;
        }
        //    } // is over a user
        //    else if(userSelection != 0) {
        //	// un hover a file
        //	if(hoverFile != 0) {
        //	    hoverFile->setMouseOver(false);
        //	    hoverFile = 0;
        //	}
        //
        //	if(userSelection != hoverUser) {
        //	    //deselect previous selection
        //	    if(hoverUser !=0) hoverUser->setMouseOver(false);
        //
        //	    //select new
        //	    userSelection->setMouseOver(true);
        //	    hoverUser = userSelection;
        //	}
    }
    else {
        if(hoverNode!=NULL) hoverNode->renderer.setMouseOver(false);
        //	if(hoverUser!=0) hoverUser->setMouseOver(false);
        hoverNode=NULL;
        //	hoverUser=0;
    }

    if(mouseleftclicked) {
        mousedragged=true;
        if(hoverNode!=NULL) selectNode(hoverNode);
        //	else if(hoverUser!=0) selectUser(hoverUser);
        else selectBackground();
    }

    if(mouserightclicked) {
        if(hoverNode!=NULL) addSelectedNode(hoverNode);
    }
}

/** Drawing */
void OroView::draw(float t, float dt) {

    Node* selectedNode = g.getSelected();

#ifndef TEXT_ONLY

    display.mode2D();

    drawBackground(dt);

    //    if(draw_loading) {
    //	loadingScreen();
    //	return;
    //    }

    Frustum frustum(camera);

    trace_time = SDL_GetTicks();

    mouseTrace(frustum,dt);

    trace_time = SDL_GetTicks() - trace_time;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    camera.focus();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#endif
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    //Draw shadows for edges and then nodes
    if (display_shadows) g.render(SHADOWS, *this, advanced_debug);

    //Draw edges and then nodes
    glBindTexture(GL_TEXTURE_2D, beamtex->textureid);
    g.render(NORMAL, *this, advanced_debug);

    //draw 'gourceian blur' around dirnodes
    glBlendFunc (GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, bloomtex->textureid);
    //Draw Bloom
    g.render(BLOOM, *this, advanced_debug);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Draw names
    if (display_labels) g.render(NAMES, *this, advanced_debug);

    //displayCoulombField(); //doesn't work?

#ifndef TEXT_ONLY


    //    glColor4f(1.0, 1.0, 0.0, 1.0);
    //    for(map<string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
    //        trace_debug ? it->second->drawSimple(dt) : it->second->draw(dt);
    //    }
    //
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

    if(advanced_debug) {

        glDisable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        track_users ? usersBounds.draw() : nodesBounds.draw();

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

    fontmedium.draw(display.width/2 - date_x_offset, 20, displaydate);

    if(debug) {
        vec3f campos = camera.getPos();

        glDisable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        track_users ? usersBounds.draw() : nodesBounds.draw();

        font.print(0,20, "FPS: %.2f", fps);
        font.print(0,40,"Time Scale: %.2f", time_scale);
        //        font.print(0,60,"Users: %d", users.size());
        font.print(0,80,"Nodes: %d", g.nodesCount());
        font.print(0,100,"Edges: %d", g.edgesCount());

        font.print(0,140,"Camera: (%.2f, %.2f, %.2f)", campos.x, campos.y, campos.z);
        font.print(0,160,"Gravity: %.2f", GRAVITY);
        font.print(0,180,"Logic Time: %u ms", logic_time);
        font.print(0,200,"Mouse Trace: %u ms", trace_time);
        font.print(0,220,"Draw Time: %u ms", SDL_GetTicks() - draw_time);

        if(hoverNode != NULL) {
            font.print(0,260,"Node %s:", hoverNode->getID().c_str());
            font.print(30,280,"Speed: (%.2f, %.2f)", hoverNode->speed.x, hoverNode->speed.y);
            font.print(30,300,"Charge: %.2f", hoverNode->charge);
            font.print(30,320,"Kinetic energy: %.2f", hoverNode->kinetic_energy);
            font.print(30,340,"Number of relations: %d", hoverNode->getRelations().size());
            font.print(30,360,"Distance to closest selected node (%s): %d",
                       (selectedNode == NULL) ? "N/A" : selectedNode->getID().c_str(),
                        hoverNode->distance_to_selected);
        }

    }

    if (display_footer) drawFooter();

    glDisable(GL_TEXTURE_2D);


    mousemoved=false;
    mouseleftclicked=false;
    mouserightclicked=false;
#endif

}

void OroView::drawBackground(float dt) {
    display.setClearColour(background_colour);
    display.clear();
}

void OroView::queueNodeInFooter(const string& id) {
    queueInFooter(g.getNode(id).renderer.getLabel());
}
void OroView::queueInFooter(const string& text) {

    int max_x = display.width + 10;

    // Ensure we do not overlap with previous text
    typedef pair<const string, int> mypair;
    BOOST_FOREACH(mypair elem, footer_content) {
        int x = elem.second + fontlarge.getWidth(elem.first);
        max_x = max(x + 50, max_x);
    }

    footer_content[text] = max_x;
}

void OroView::drawFooter() {

    glColor4f(1.0f, 1.0f, 0.8f, 0.7f);

    for( map<string, int>::iterator iter = footer_content.begin() ;
         iter != footer_content.end() ;
         ) {

        if (iter->second < -fontlarge.getWidth(iter->first)) {
            map<string, int>::iterator elem_to_remove = iter++;
            footer_content.erase(elem_to_remove);
            continue;
        }

        fontlarge.print(iter->second,display.height - fontlarge.getFontSize() - 20, "%s", iter->first.c_str());

        iter->second -= FOOTER_SPEED;

        iter++;
    }

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

void OroView::drawVector(vec2f vec, vec2f pos, vec4f col) {

    float radius = 5.0;

    //conversion not optimal
    float angle = atan2(vec.y, vec.x) * 180 / 3.1415926;

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0f);
    glRotatef(angle, 0.0, 0.0, 1.0);

    glColor4fv(col);

    glBegin(GL_QUADS);

    // src point

    glTexCoord2f(0.0,0.0);
    glVertex2f(0, -radius/3);
    glTexCoord2f(1.0,0.0);
    glVertex2f(0, radius/3);

    // dest point
    glTexCoord2f(1.0,0.0);
    glVertex2f(vec.length() - 5.0, radius/3);
    glTexCoord2f(0.0,0.0);
    glVertex2f(vec.length() - 5.0, -radius/3);


    glEnd();

    //Arrow
    glBegin(GL_TRIANGLES);

    glTexCoord2f(0.0,0.0);
    glVertex2f(vec.length() - radius, -radius/2);
    glTexCoord2f(0.5,1.0);
    glVertex2f(vec.length(), 0);
    glTexCoord2f(1.0,0.0);
    glVertex2f(vec.length() - radius, radius/2);

    glEnd();

    glPopMatrix();
}

void OroView::displayCoulombField() {
    for (float i = -200.0; i < 200.0; i += 40.0) {
        for (float j = -200.0; j < 200.0; j += 40.0) {
            vec2f pos(i, j);

            drawVector(g.coulombRepulsionAt(pos), pos, (1.0, 1.0, 0.2, 0.7));
        }
    }
}
/** Camera */
void OroView::updateCamera(float dt) {

    //camera tracking

    Node* selectedNode = g.getSelected();

    if(backgroundSelected) {
        Bounds2D mousebounds;
        mousebounds.update(backgroundPos);

        camera.adjust(mousebounds);

    } else if(track_users && (selectedNode !=NULL || selectedUser !=NULL)) {
        Bounds2D focusbounds;

        vec3f camerapos = camera.getPos();

        //	if(selectedUser !=NULL) focusbounds.update(selectedUser->getPos());
        if(selectedNode !=NULL) focusbounds.update(selectedNode->pos);

        camera.adjust(focusbounds);
    } else {
        if(track_users && idle_time==0) camera.adjust(usersBounds);
        else camera.adjust(nodesBounds);
    }

    camera.logic(dt);
}

void OroView::setCameraMode(bool track_users) {
    this->track_users = track_users;
    if(selectedUser!=NULL) camera.lockOn(track_users);
    backgroundSelected=false;
}

void OroView::zoom(bool zoomin) {

    float min_distance = camera.getMinDistance();
    float max_distance = camera.getMaxDistance();

    float zoom_multi = 1.1;

    if(zoomin) {
        min_distance /= zoom_multi;
        if(min_distance < 100.0) min_distance = 100.0;

        camera.setMinDistance(min_distance);
    } else {
        min_distance *= zoom_multi;
        if(min_distance > 1000.0) min_distance = 1000.0;

        camera.setMinDistance(min_distance);
    }
}

/** Background */
void OroView::setBackground(vec3f background) {
    background_colour = background;
}

//trace click of mouse on background
void OroView::selectBackground() {

    backgroundSelected = true;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluPerspective(90.0f, (GLfloat)display.width/(GLfloat)display.height, 0.1f, -camera.getPos().z);
    camera.look();

    vec2f screen_centre(display.width*0.5,display.height*0.5);

    backgroundPos = display.unproject(screen_centre).truncate();

    SDL_ShowCursor(false);
    SDL_WM_GrabInput(SDL_GRAB_ON);

}

/** Nodes */
//select a node, deselect current node
void OroView::selectNode(Node* node) {

    if (node->selected) return;

    backgroundSelected=false;

    g.clearSelect();
    g.select(node);


    queueNodeInFooter(node->getID());
    updateCurrentNode();
}

//select a node, keep currently selected node
void OroView::addSelectedNode(Node* node) {

    backgroundSelected=false;

    if (node->selected) g.deselect(node);
    else {
        g.select(node);
        queueNodeInFooter(node->getID());
    }
}

void OroView::addAlias(const string& alias, const string& id) {
    g.addAlias(alias, id);
}

//Add node
bool OroView::addNodeConnectedTo(const string& id,
                                 const string& node_label,
                                 const string& to,
                                 relation_type type,
                                 const string& edge_label){

    Node* n;
    Node* neighbour;

    string label = node_label;

    try {
        neighbour = &g.getNode(to);
    }
    catch(OroViewException& exception) {
        //neighbour not found, create it.
        TRACE("Neighbour " << to << " not found. Creating it.");

        addNodeConnectedTo(to, to, ROOT_CONCEPT, UNDEFINED, "");
        neighbour = &g.getNode(to);
    }

    try {
        //does the node already exists?

        //if yes, reuse it
        n = &g.getNode(id);
    }
    catch(OroViewException& exception) {
        //if not, create it, create it.
        TRACE("Not existing myself (" << id << "). Creating myself.");

        node_type ntype;

        //guess the type of the node we are adding
        switch (type) {
        case SUBCLASS:
        case SUPERCLASS:
        case CLASS:
            ntype = CLASS_NODE;
            break;

        case INSTANCE:
            ntype = INSTANCE_NODE;
            break;

        case PROPERTY:
        case OBJ_PROPERTY:
        case DATA_PROPERTY:
            if (boost::starts_with(id, "literal")) {
                if (node_label == "true"){
                    ntype = TRUE_NODE;
                    label = "";
                }
                else if (node_label == "false"){
                    ntype = FALSE_NODE;
                    label = "";
                }
                else ntype = LITERAL_NODE;

            }
            else ntype = INSTANCE_NODE;
            break;

        case COMMENT:
            ntype = COMMENT_NODE;
            break;

        default:
            TRACE("Default type of node? strange...");
            ntype = INSTANCE_NODE;
        }

        if (only_labelled_nodes &&
            ntype == CLASS_NODE &&
            label == id) { //no a very robust way to check if a node has a label, but it's fast

            TRACE("Node " << id << " has no label, discarding it.");
            return false;
        }

        n = &g.addNode(id, label, neighbour, ntype);
    }

    g.addEdge(*n, *neighbour, type, edge_label);

    return true;
}

Node& OroView::getNode(const std::string &id) {
    return g.getNode(id);
}

void OroView::updateCurrentNode() {
    Node* selectedNode = g.getSelected();

    if (selectedNode != NULL) {
        TRACE("Updating node " << selectedNode->getID());
        oro.walkThroughOntology(selectedNode->getID(), 1, this);
    }
    else cerr << "Select only one node to expand it." << endl;
}

/** Testing */
void OroView::addRandomNodes(int amount,int nb_rel) {

    const int length = 6; //length of randomly created ID.

    for (int i = 0; i < amount ; ++i) {

        string newId;

        //Generate a new random id for this node
        for(int j=0; j<length; ++j)
        {
            newId += (char)(rand() % 26 + 97); //ASCII codes of letters starts at 98 for "a"
        }

        Node& neighbour = g.getRandomNode();

        Node& n = g.addNode(newId, newId, &neighbour);
        vec4f col((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, 0.7);
        n.setColour(col);

        g.addEdge(n, neighbour, SUBCLASS, "voisin");

        for(int k=0; k<(nb_rel - 1); ++k) {

            //We may pick ourselves, but it's not that a problem
            Node& n2 = g.getRandomNode();
            g.addEdge(n, n2, SUBCLASS, "test");
        }
    }
}
