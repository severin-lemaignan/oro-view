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

#include "node_renderer.h"
#include "macros.h"
#include "oroview.h"

using namespace std;

NodeRenderer::NodeRenderer(int tagid, string label, node_type type) :
    tagid(tagid),
    label(label),
    type(type)
{

    idle_time = 0.0;

    size = NODE_SIZE;

    col = vec4f(1.0, 1.0, 1.0, 1.0);

    hovered = false;
    selected = false;

#ifndef TEXT_ONLY
    if (type == CLASS_NODE) {
        base_col = CLASSES_COLOUR;
        icon = texturemanager.grab("classes.png");
    }
    else if (type == INSTANCE_NODE) {
        base_col = INSTANCES_COLOUR;
        icon = texturemanager.grab("instances.png");
    }
    else if (type == LITERAL_NODE) {
        base_col = LITERAL_COLOUR;
        icon = texturemanager.grab("literals.png");
    }
    else if (type == COMMENT_NODE) {
        base_col = LITERAL_COLOUR;
        icon = texturemanager.grab("comment.png");
    }
    else if (type == TRUE_NODE) {
        base_col = vec4f(0.2, 1.0, 0.2, 1.0); //green
        icon = texturemanager.grab("yes.png");
    }
    else if (type == FALSE_NODE) {
        base_col = vec4f(1.0, 0.2, 0.2, 1.0); //red
        icon = texturemanager.grab("no.png");
    }
    else {
        base_col = vec4f(1.0, 1.0, 1.0, 1.0);
        icon = texturemanager.grab("instances.png");
    }
#endif
}

void NodeRenderer::setColour(vec4f col) {
    base_col = col;
}

void NodeRenderer::setRenderingColour() {
    if (selected) col = SELECTED_COLOUR;
    else {
        if (hovered) col = HOVERED_COLOUR;
        else col = base_col;
    }
}

float NodeRenderer::getAlpha(int distance) {

    if (distance <= 1) return 1.0f;

    return std::max(0.0f, FADE_TIME - (idle_time * std::max(1, distance)))/FADE_TIME;
}

void NodeRenderer::increment_idle_time(float dt) {
    if (selected || hovered) idle_time = 0.0;
    else idle_time += dt;
}

void NodeRenderer::draw(const vec2f& pos, rendering_mode mode, OroView& env, int distance_to_selected) {

    switch (mode) {

    case NORMAL:
    case SIMPLE:
        drawSimple(pos);
        break;

    case NAMES:
        drawName(pos, env.font, distance_to_selected);

        break;

    case BLOOM:
        drawBloom(pos, env.camera);
        break;

    case SHADOWS:
        drawShadow(pos);
        break;
    }


}


void NodeRenderer::drawSimple(const vec2f& pos){

    setRenderingColour();

    float node_size = selected ? size * SELECT_SIZE_FACTOR : size;

    glLoadName(tagid);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    float ratio = icon->h / (float) icon->w;
    float halfsize = node_size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize);

    glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

    glPushMatrix();
    glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

    glColor4fv(col);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f,0.0f);
    glVertex2f(node_size, 0.0f);

    glTexCoord2f(1.0f,1.0f);
    glVertex2f(node_size, node_size*ratio);

    glTexCoord2f(0.0f,1.0f);
    glVertex2f(0.0f, node_size*ratio);
    glEnd();

    glPopMatrix();

    glLoadName(0);

}

void NodeRenderer::drawName(const vec2f& pos, FXFont& font, int distance_to_selected){

    float alpha = getAlpha(distance_to_selected);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0);

    glColor4f(1.0, 1.0, 1.0, alpha);

    vec3f screenpos = display.project(vec3f(0.0, 0.0, 0.0));

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, display.width, display.height, 0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    font.draw(screenpos.x, screenpos.y, label);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopMatrix();
}

void NodeRenderer::drawBloom(const vec2f& pos, ZoomCamera& camera){

    Frustum frustum(camera);

    //if(isVisible() && frustum.boundsInFrustum(quadItemBounds)) {

    float bloom_radius = 50.0;

    vec4f bloom_col = col;

    glColor4f(bloom_col.x, bloom_col.y, bloom_col.z, 1.0);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0);

    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(bloom_radius,bloom_radius);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(bloom_radius,-bloom_radius);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(-bloom_radius,-bloom_radius);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(-bloom_radius,bloom_radius);
    glEnd();
    glPopMatrix();

    //}

}

void NodeRenderer::drawShadow(const vec2f& pos){

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize) + SHADOW_OFFSET;;

    glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

    glPushMatrix();
    glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

    glColor4f(0.0, 0.0, 0.0, SHADOW_STRENGTH);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f,0.0f);
    glVertex2f(size, 0.0f);

    glTexCoord2f(1.0f,1.0f);
    glVertex2f(size, size*ratio);

    glTexCoord2f(0.0f,1.0f);
    glVertex2f(0.0f, size*ratio);
    glEnd();

    glPopMatrix();

}


void NodeRenderer::setMouseOver(bool over) {
    hovered = over;

}

void NodeRenderer::setSelected(bool select) {
    selected = select;
}
