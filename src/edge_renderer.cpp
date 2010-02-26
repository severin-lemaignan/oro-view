#include "edge_renderer.h"

#include "node.h"

EdgeRenderer::EdgeRenderer(int tagid) : tagid(tagid)
{
}

void EdgeRenderer::render() {

    spline.draw();
}

void EdgeRenderer::updateSpline(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos){

    vec2f projected_pos1  = display.project(vec3f(pos1.x, pos1.y, 0.0)).truncate();
    vec2f projected_pos2  = display.project(vec3f(pos2.x, pos2.y, 0.0)).truncate();
    vec2f projected_spos = display.project(vec3f(spos.x, spos.y, 0.0)).truncate();

    spline = SplineEdge(projected_pos1, col1, projected_pos2, col2, spos);
}
