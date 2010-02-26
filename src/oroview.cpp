#include <string>

#include "macros.h"
#include "oroview.h"

#include "node_relation.h"

#include "graph.h"

using namespace std;

OroView::OroView()
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
    selectedNode = NULL;

    track_users = false;
    selectedUser = NULL;

    //Mouse

    mousemoved = false;
    mouseclicked = false;
    mousedragged = false;

    backgroundSelected = false;

    background_colour = vec3f(0.0, 0.0, 0.0);

    draw_loading = true;
    debug = true;
    paused = false;

    fontlarge = fontmanager.grab("FreeSans.ttf", 42);
    fontlarge.dropShadow(true);
    fontlarge.roundCoordinates(true);

    fontmedium = fontmanager.grab("FreeSans.ttf", 16);
    fontmedium.dropShadow(true);
    fontmedium.roundCoordinates(false);

    font = fontmanager.grab("FreeSans.ttf", 14);
    font.dropShadow(true);
    font.roundCoordinates(true);

    camera = ZoomCamera(vec3f(0,0, -300), vec3f(0.0, 0.0, 0.0), 250.0, 5000.0);

#ifndef TEXT_ONLY
    bloomtex = texturemanager.grab("bloom.tga");
    beamtex  = texturemanager.grab("beam.png");
#endif
}

/** Initialization */
void OroView::init(){

    TRACE("*** Initialization ***");

    Graph& g = *(Graph::getInstance());


    Node& a = g.addNode("node1");
    Node& b = g.addNode("node2");
    a.addRelation(b, INSTANCE, "loves");

   // Node& c =  g.addNode("node3");
    //g.addNode("node4");

    //a.addRelation(c, INSTANCE, "hates");
    //b.addRelation(c, INSTANCE, "loves");

    TRACE("\t - Graph created and populated");
}

/** Events */
void OroView::keyPress(SDL_KeyboardEvent *e) {
    if (e->type == SDL_KEYUP) return;

    if (e->type == SDL_KEYDOWN) {
	if (e->keysym.sym == SDLK_ESCAPE) {
	    appFinished=true;
	}

	if (e->keysym.sym == SDLK_d) {
	    debug = !debug;
	}

//        if (e->keysym.sym == SDLK_w) {
//            gGourceQuadTreeDebug = !gGourceQuadTreeDebug;
//        }

	if (e->keysym.sym == SDLK_c) {
	    toggleCameraMode();
	}

	if (e->keysym.sym == SDLK_SPACE) {
	    addRandomNodes(2, 1);
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
	toggleCameraMode();
	return;
    }

    if(e->button == SDL_BUTTON_LEFT) {

	mousepos = vec2f(e->x, e->y);
	mouseclicked=true;
    }
}

void OroView::mouseMove(SDL_MouseMotionEvent *e) {

    //move camera in direction the user dragged the mouse
    if(mousedragged) {
	backgroundPos += vec2f( e->xrel, e->yrel );

	return;
    }

    mouse_inactivity = 0.0;
    SDL_ShowCursor(true);

    mousepos = vec2f(e->x, e->y);
    mousemoved=true;
}

/** Time update */
void OroView::update(float t, float dt) {

    SDL_Delay(50); //N'allons pas trop vite au début...

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

     Graph& g = *(Graph::getInstance());

     g.resetRenderers();

    //still want to update camera while paused
    if(paused) {
	updateCamera(dt);
	return;
    }

    //check if mouse has been inactive for 3 seconds
    //and if so hide it.
    if(!mouseclicked && mouse_inactivity<3.0) {
	mouse_inactivity += dt;

	if(mouse_inactivity>=3.0) {
	    SDL_ShowCursor(false);
	}
    }

    g.step(dt);

     updateCamera(dt);
}

void OroView::mouseTrace(Frustum& frustum, float dt) {

    Graph& g = *(Graph::getInstance());

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

    g.render(false);
    g.resetRenderers();

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
	    nodeSelection = Graph::getInstance()->getNodeByTagID(choice);

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

    if(mouseclicked) {
	if(hoverNode!=NULL) selectNode(hoverNode);
//	else if(hoverUser!=0) selectUser(hoverUser);
	else {
	    selectBackground();
	}
    }
}

/** Drawing */
void OroView::draw(float t, float dt) {
    Graph& g = *(Graph::getInstance());

#ifndef TEXT_ONLY

    display.mode2D();

    drawBackground(dt);

    if(draw_loading) {
	loadingScreen();
	return;
    }

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

     g.render();

#ifndef TEXT_ONLY
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

    if(debug) {

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
	font.print(0,80,"Nodes: %d", Graph::getInstance()->nodesCount());
	font.print(0,100,"Edges: %d", Graph::getInstance()->edgesCount());

	font.print(0,140,"Camera: (%.2f, %.2f, %.2f)", campos.x, campos.y, campos.z);
	font.print(0,160,"Gravity: %.2f", GRAVITY);
	font.print(0,180,"Logic Time: %u ms", logic_time);
	font.print(0,200,"Mouse Trace: %u ms", trace_time);
	font.print(0,220,"Draw Time: %u ms", SDL_GetTicks() - draw_time);

	if(selectedNode != NULL) {
	    font.print(0,260,"%s selected.", selectedNode->getID().c_str());
	    font.print(30,280,"Speed: (%.2f, %.2f)", selectedNode->speed.x, selectedNode->speed.y);
	    font.print(30,300,"Charge: %.2f", selectedNode->charge);
	    font.print(30,320,"Kinetic energy: %.2f", selectedNode->kinetic_energy);
	    font.print(30,340,"Number of relations: %d", selectedNode->getRelations().size());
	}

    }

    glDisable(GL_TEXTURE_2D);


    mousemoved=false;
    mouseclicked=false;
#endif

}

void OroView::drawBackground(float dt) {
    display.setClearColour(background_colour);
    display.clear();
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

/** Camera */
void OroView::updateCamera(float dt) {

    //camera tracking

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

void OroView::toggleCameraMode() {
    setCameraMode(!track_users);
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

    mousedragged=true;
}

/** Nodes */
//select a file, deselect current file/user
void OroView::selectNode(Node* node) {

    //already selected do nothing
    if(node!=NULL && selectedNode==node) return;

    backgroundSelected=false;
//
//    if(selectedUser != 0) {
//        selectedUser->setSelected(false);
//        selectedUser = 0;
//    }

    // deselect current node
    if(selectedNode != NULL) {
	selectedNode->setSelected(false);
	selectedNode = NULL;
    }

    //if no file return
    if(node == NULL) {
	return;
    }

    selectedNode = node;

    //select node, lock on camera
    selectedNode->setSelected(true);
}

/** Testing */
void OroView::addRandomNodes(int amount,int nb_rel) {

    const int length = 6; //length of randomly created ID.

    Graph& g = *(Graph::getInstance());

    for (int i = 0; i < amount ; ++i) {

	string newId;

	//Generate a new random id for this node
	for(int j=0; j<length; ++j)
	{
		newId += (char)(rand() % 26 + 97); //ASCII codes of letters starts at 98 for "a"
	}

	Node& n = g.addNode(newId);

	for(int k=0; k<nb_rel; ++k) {

	    n.addRelation(g.getRandomNode(), SUBCLASS, "test");
	}
    }
}
