#ifndef NODE_RENDERER_H
#define NODE_RENDERER_H

#include "styles.h"
#include "core/vectors.h"
#include "core/texture.h"

class OroView;

class NodeRenderer
{

    std::string label;

    int tagid;

    TextureResource* icon;
    float size;

    float elapsed;
    float fadetime;

    float getAlpha() { return std::min(elapsed/fadetime, 1.0f); }


    TextureResource* getIcon() { return icon; }

    bool hovered;
    bool selected;

    vec4f base_col;

    void setRenderingColour();

public:
    NodeRenderer(int tagid, std::string label);

    vec4f col;

    void draw(const vec2f& pos, rendering_mode mode, OroView& env);

    void setMouseOver(bool over);
    void setSelected(bool selected);
    void setColour(vec4f col);



};

#endif // NODE_RENDERER_H
