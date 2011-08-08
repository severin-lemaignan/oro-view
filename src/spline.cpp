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

#include "constants.h"
#include "macros.h"

#include "spline.h"
#include "styles.h"


SplineEdge::SplineEdge() {
}

SplineEdge::SplineEdge(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos, bool arrow_head, bool arrow_tail) :
    arrow_head(arrow_head),
    arrow_tail(arrow_tail)
{

    vec2f pt_last;
    vec4f col_last;

    vec2f mid = (pos1 - pos2) * 0.5;
    vec2f to  = vec2f(pos1 - spos);

    float dp = std::min(1.0f, to.normal().dot(mid.normal()));

    float ang = acos(dp) / PI;

    // max 10,
    int max_detail = 10;

    int edge_detail = std::min(max_detail, (int) (ang * 100.0));
    if(edge_detail<1.0) edge_detail = 1.0;

    spline_point.reserve(edge_detail+1);
    spline_colour.reserve(edge_detail+1);

    //calculate positions
    for(int i=0; i <= edge_detail; i++) {
        float t = (float)i/edge_detail;
        float tt = 1.0f-t;

        vec2f p0 = pos1 * t + spos * tt;
        vec2f p1 = spos * t + pos2 * tt;

        vec2f pt = p0 * t + p1 * tt;

        vec4f coln = col1 * t + col2 * tt;

        spline_point.push_back(pt);
        spline_colour.push_back(coln);
    }
}

void SplineEdge::drawBeam(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, float radius, bool first, bool last) {

    vec2f perp = (pos1 - pos2).perpendicular().normal() * radius;

    if (first) {

        if (arrow_head) {

            vec2f arrow = (pos2 - pos1).normal() * ARROW_SIZE;

            vec2f newpos1 = pos1 + arrow;

            glEnd();
            glBegin(GL_TRIANGLES);

            glColor4fv(col1);
            glTexCoord2f(0.0,0.0);
            glVertex2f(newpos1.x + perp.x * 2, newpos1.y + perp.y * 2);
            glTexCoord2f(0.5,1.0);
            glVertex2f(pos1.x, pos1.y);
            glTexCoord2f(1.0,0.0);
            glVertex2f(newpos1.x - perp.x * 2, newpos1.y - perp.y * 2);

            glEnd();
            glBegin(GL_QUAD_STRIP);
            // src point
            glTexCoord2f(1.0,0.0);
            glVertex2f(newpos1.x + perp.x, newpos1.y + perp.y);
            glTexCoord2f(0.0,0.0);
            glVertex2f(newpos1.x - perp.x, newpos1.y - perp.y);

        }
        else {
            // src point
            glColor4fv(col1);
            glTexCoord2f(1.0,0.0);
            glVertex2f(pos1.x + perp.x, pos1.y + perp.y);
            glTexCoord2f(0.0,0.0);
            glVertex2f(pos1.x - perp.x, pos1.y - perp.y);
        }
    }


    // dest point
    glColor4fv(col2);

    if (last && arrow_tail) {

        vec2f arrow = (pos2 - pos1).normal() * ARROW_SIZE;

        vec2f newpos2 = pos2 - arrow;

        glTexCoord2f(1.0,0.0);
        glVertex2f(newpos2.x + perp.x, newpos2.y + perp.y);
        glTexCoord2f(0.0,0.0);
        glVertex2f(newpos2.x - perp.x, newpos2.y - perp.y);
        //Arrow
        glEnd();
        glBegin(GL_TRIANGLES);

        glTexCoord2f(0.0,0.0);
        glVertex2f(newpos2.x + perp.x * 2, newpos2.y + perp.y * 2);
        glTexCoord2f(0.5,1.0);
        glVertex2f(pos2.x, pos2.y);
        glTexCoord2f(1.0,0.0);
        glVertex2f(newpos2.x - perp.x * 2, newpos2.y - perp.y * 2);
    }
    else {
        glTexCoord2f(1.0,0.0);
        glVertex2f(pos2.x + perp.x, pos2.y + perp.y);
        glTexCoord2f(0.0,0.0);
        glVertex2f(pos2.x - perp.x, pos2.y - perp.y);
    }
}

void SplineEdge::drawShadow() {

    int edges_count = spline_point.size() - 1;

    glBegin(GL_QUAD_STRIP);

    for(int i=0;i<edges_count;i++) {
        drawBeam(spline_point[i] + SHADOW_OFFSET, vec4f(0.0, 0.0, 0.0, SHADOW_STRENGTH * spline_colour[i].w),
                 spline_point[i+1] + SHADOW_OFFSET, vec4f(0.0, 0.0, 0.0, SHADOW_STRENGTH * spline_colour[i+1].w),
                 2.5,
                 i == 0, // first?
                 i == edges_count - 1 // last?
                 );
    }

    glEnd();
}

void SplineEdge::draw() {

    int edges_count = spline_point.size() - 1;

    glBegin(GL_QUAD_STRIP);

    for(int i=0;i<edges_count;i++) {
        drawBeam(spline_point[i], spline_colour[i],
                 spline_point[i+1], spline_colour[i+1],
                 1.5,
                 i == 0, //first?
                 i == edges_count - 1 //last?
                 );
    }

    glEnd();
}
