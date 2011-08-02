/*
    Copyright (c) 2010 Séverin Lemaignan (slemaign@laas.fr)

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

#ifndef STYLES_H
#define STYLES_H

#include "core/vectors.h"

enum rendering_mode {NORMAL, SIMPLE, SHADOWS, BLOOM, NAMES};

static const float NODE_SIZE = 15.0;
static const float SELECT_SIZE_FACTOR = 1.5; //selected node will appear SELECT_SIZE_FACTOR bigger.

static const int MAX_NODE_LEVELS = 6; //how many levels of nodes are displayed around the selected one?

static const float FADE_TIME = 25.0; //idle time (in sec) before labels vanish
static const float COLOUR_DECAY_TIME = 2.0; //idle time (in sec) before labels vanish
static const float SHADOW_STRENGTH = 0.5; //intensity of shadows (from 0.0 to 1.0)
static const vec2f SHADOW_OFFSET(1.0, 1.0); //offset of shadows

//static const vec4f HOVERED_COLOUR(0.8, 0.2, 0.5, 1.0); // Rose
static const vec4f HOVERED_COLOUR(0.55, 0.1, 0.1, 1.0); // Dark red
//static const vec4f SELECTED_COLOUR(0.2, 0.8, 0.5, 1.0); //Turquoise
static const vec4f SELECTED_COLOUR(0.99, 0.69, 0.24, 1.0); //Bright orange

// "EGA" color scheme
//static const vec4f CLASSES_COLOUR(0.58, 0.82, 0.86, 1.0);
//static const vec4f INSTANCES_COLOUR(0.72, 0.55, 0.79, 1.0);
//static const vec4f LITERAL_COLOUR(0.57, 0.3, 0.48, 1.0);

// "Prune/Kaki" color scheme
static const vec4f CLASSES_COLOUR(0.36, 0.21, 0.4, 1.0);
static const vec4f INSTANCES_COLOUR(0.32, 0.4, 0.21, 1.0);
static const vec4f LITERAL_COLOUR(0.21, 0.4, 0.25, 1.0);

#endif // STYLES_H
