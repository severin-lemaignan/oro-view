#ifndef EDGE_RENDERER_H
#define EDGE_RENDERER_H

#include "core/vectors.h"

#include "styles.h"
#include "spline.h"

class EdgeRenderer
{
    int tagid;

    SplineEdge spline;

public:
    EdgeRenderer(int tagid);

    void render(rendering_mode mode);

    void update(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos);


};

#endif // EDGE_RENDERER_H
