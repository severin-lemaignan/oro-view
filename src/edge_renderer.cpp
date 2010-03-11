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

EdgeRenderer::EdgeRenderer(int tagid) : tagid(tagid)
{
}

void EdgeRenderer::render(rendering_mode mode) {

    switch (mode) {
    case NORMAL:
        spline.draw();
        break;
    case SHADOWS:
        spline.drawShadow();
        break;
    }


}

void EdgeRenderer::update(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos){

    vec2f projected_pos1  = display.project(vec3f(pos1.x, pos1.y, 0.0)).truncate();
    vec2f projected_pos2  = display.project(vec3f(pos2.x, pos2.y, 0.0)).truncate();
    vec2f projected_spos = display.project(vec3f(spos.x, spos.y, 0.0)).truncate();

    spline = SplineEdge(projected_pos1, col1, projected_pos2, col2, spos);

}
