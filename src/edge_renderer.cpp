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

#include "edge_renderer.h"

#include "node.h"
#include "oroview.h"

using namespace std;

EdgeRenderer::EdgeRenderer(int tagid, const string& label) :
    tagid(tagid),
    label(label),
    label_pos(vec2f(0.0, 0.0)),
    idle_time(0.0),
    current_distance_to_selected(-1)
{
}

float EdgeRenderer::getAlpha() {

    //This edge is connected to the selected node, don't fade it away.
    if (selected) return 1.0f;

    return std::max(0.0f, FADE_TIME - (idle_time * std::max(1, current_distance_to_selected)))/FADE_TIME;
}

void EdgeRenderer::draw(rendering_mode mode, OroView& env, int distance_to_selected) {

    current_distance_to_selected = distance_to_selected;

    switch (mode) {
    case NORMAL:
        spline.draw();
        break;

    case NAMES:
        drawName(env.font);
        break;

    case SHADOWS:
        spline.drawShadow();
        break;
    }


}

void EdgeRenderer::update(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos){

    label_pos = pos1 + (pos2 - pos1) * 0.5;

    vec2f projected_pos1  = display.project(vec3f(pos1.x, pos1.y, 0.0)).truncate();
    vec2f projected_pos2  = display.project(vec3f(pos2.x, pos2.y, 0.0)).truncate();
    vec2f projected_spos = display.project(vec3f(spos.x, spos.y, 0.0)).truncate();

    spline = SplineEdge(projected_pos1, col1, projected_pos2, col2, spos);

}

void EdgeRenderer::increment_idle_time(float dt) {
    if (selected) idle_time = 0.0;
    else idle_time += dt;
}

void EdgeRenderer::drawName(FXFont& font){

    glPushMatrix();
    glTranslatef(label_pos.x, label_pos.y, 0.0);

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
