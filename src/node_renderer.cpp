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
    type(type),
    idle_time(0.0),
    decayTime(0.0),
    decaySpeed(1.0),
    decaying(true),
    hovered(false),
    selected(false),
    current_distance_to_selected(-1),
    base_size(NODE_SIZE)
{

    size = base_size * 1.2;

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

    col = base_col * 1.2;
#endif


}

void NodeRenderer::setColour(vec4f col) {
    base_col = col;
}

void NodeRenderer::computeColourSize() {
    if (selected) {
        col = SELECTED_COLOUR;
        size = base_size * SELECT_SIZE_FACTOR;
    }
    else {
        base_size = NODE_SIZE * max(0.6f, getAlpha()); //scales nodes depending on their 'visibility' (mix of idle time + distance to selected node)
        decay();

        if (hovered) col = HOVERED_COLOUR;
    }
}

void NodeRenderer::decay() {

    if (  abs(col.x - base_col.x) > 0.001
       || abs(col.y - base_col.y) > 0.001
       || abs(col.z - base_col.z) > 0.001 //do not compare alpha values
       || abs(size - base_size) > 0.001 ) {
        decaying = true;

        // At start, ratio = 1.0 => 100% 'new color/size'
        // At end, ratio = 0.0 => 100% 'base color/size'
        float decayRatio = 1.0 - decayTime / (COLOUR_DECAY_TIME * 100.0 / decaySpeed);

        if (decayRatio < 0.0) {
            // End of decay period
            decaying = false;
            decayTime = 0.0;
            decaySpeed = 1.0;
            col = base_col;
            size = base_size;
            return;
        }

        col = base_col + ((col - base_col) * decayRatio);
        size = base_size + ((size - base_size) * decayRatio);

        return;

    }

    // Default: no decaying
    decaying = false;
    col = base_col;
    size = base_size;
}

float NodeRenderer::getAlpha() {

    if (current_distance_to_selected <= 1) return 1.0f;

    int distance = std::max(1, current_distance_to_selected);
    //float level =  FADE_TIME - (idle_time * distance)/FADE_TIME;

    // We use a function in (1-x^3) to smooth the fading of nodes
    #define MAX_DISTANCE_3 (MAX_NODE_LEVELS * MAX_NODE_LEVELS * MAX_NODE_LEVELS)

    float fading = (FADE_TIME - idle_time) / FADE_TIME;
    float level =  1.0f - (std::pow(distance, 3)/MAX_DISTANCE_3);

    return std::max(0.0f, level * fading);
}

void NodeRenderer::increment_idle_time(float dt) {
    if (selected || hovered || current_distance_to_selected <= 1) idle_time = 0.0;
    else idle_time += dt;

    if (decaying) decayTime += dt;
}

void NodeRenderer::draw(const vec2f& pos, rendering_mode mode, OroView& env, int distance_to_selected) {

    current_distance_to_selected = distance_to_selected;


    switch (mode) {

    case NORMAL:
    case SIMPLE:
        computeColourSize();

        drawSimple(pos);
        break;

    case NAMES:
        drawName(pos, env.font);

        break;

    case BLOOM:
        drawBloom(pos);
        break;

    case SHADOWS:
        drawShadow(pos);
        break;
    }


}


void NodeRenderer::drawSimple(const vec2f& pos){


    glLoadName(tagid);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize);

    glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

    glPushMatrix();
    glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

    col.w = getAlpha();

    glColor4fv(col);

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

    glLoadName(0);

}

void NodeRenderer::drawName(const vec2f& pos, FXFont& font){

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0);

    glColor4f(1.0, 1.0, 1.0, getAlpha());

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

void NodeRenderer::drawBloom(const vec2f& pos){

    float bloom_radius = 50.0;

    vec4f bloom_col = col;

    glColor4f(bloom_col.x, bloom_col.y, bloom_col.z, getAlpha());

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

    glColor4f(0.0, 0.0, 0.0, std::min(SHADOW_STRENGTH, getAlpha()));

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
