#ifndef NODE_RENDERER_H
#define NODE_RENDERER_H

#include "core/sdlapp.h"

class NodeRenderer
{
    int tagid;

    TextureResource* icon;
    float size;

    float elapsed;
    float fadetime;

    float getAlpha() { return std::min(elapsed/fadetime, 1.0f); }


    TextureResource* getIcon() { return icon; }

    bool hovered;
    bool selected;

public:
    NodeRenderer(int tagid);

    vec4f col;

    void renderAt(const vec2f& pos);

    void setMouseOver(bool over);
    void setSelected(bool selected);



};

#endif // NODE_RENDERER_H
