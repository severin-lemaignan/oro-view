#ifndef NODE_RENDERER_H
#define NODE_RENDERER_H

#include "styles.h"
#include "core/vectors.h"
#include "core/texture.h"
#include "zoomcamera.h"

class OroView;

class NodeRenderer
{

    float idle_time;

    std::string label;

    int tagid;

    TextureResource* icon;
    float size;

    float getAlpha() { return std::max(0.0f, FADE_TIME - idle_time)/FADE_TIME; }


    TextureResource* getIcon() { return icon; }

    bool hovered;
    bool selected;

    vec4f base_col;

    void setRenderingColour();

    void drawSimple(const vec2f& pos);
    void drawName(const vec2f& pos, FXFont& font);
    void drawBloom(const vec2f& pos, ZoomCamera& camera);
    void drawShadow(const vec2f& pos);


public:
    NodeRenderer(int tagid, std::string label);

    vec4f col;

    void draw(const vec2f& pos, rendering_mode mode, OroView& env);

    /**
    If the node is not selected, will increment the idle time of this
    node renderer by dt.
    */
    void increment_idle_time(float dt);

    void setMouseOver(bool over);
    void setSelected(bool selected);
    void setColour(vec4f col);



};

#endif // NODE_RENDERER_H
