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

#ifndef NODE_RENDERER_H
#define NODE_RENDERER_H

#include "styles.h"
#include "constants.h"
#include "core/vectors.h"
#include "core/texture.h"
#include "zoomcamera.h"

class OroView;

class NodeRenderer
{

    float idle_time;

    std::string label;

    int tagid;

    node_type type;

    TextureResource* icon;

    float getAlpha();


    TextureResource* getIcon() { return icon; }

    bool hovered;
    bool selected;
    int current_distance_to_selected;

    vec4f base_col;
    float base_size;
    int base_fontsize;

    void computeColourSize();
    void decay();

    void computeSize();

    void drawSimple(const vec2f& pos);
    void drawName(const vec2f& pos, FXFont& font);
    void drawBloom(const vec2f& pos);
    void drawShadow(const vec2f& pos);


public:
    NodeRenderer(int tagid, std::string label, node_type type = CLASS_NODE);

    vec4f col;
    float size;
    float fontsize;
    float decayRatio;


    void draw(const vec2f& pos, rendering_mode mode, OroView& env, int distance_to_selected = -1);

    /**
    If the node is not selected, will increment the idle time of this
    node renderer by dt.
    */
    void increment_idle_time(float dt);

    void setMouseOver(bool over);
    void setSelected(bool selected);
    void setColour(vec4f col);

    std::string getLabel() {return label;}



};

#endif // NODE_RENDERER_H
