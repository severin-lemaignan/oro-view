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

static const float FADE_TIME = 5.0; //idle time (in sec) before labels vanish
static const float SHADOW_STRENGTH = 0.5; //intensity of shadows (from 0.0 to 1.0)
static const vec2f SHADOW_OFFSET(1.0, 1.0); //offset of shadows

static const vec4f HOVERED_COLOUR(1.0, 0.2, 0.5, 1.0);
static const vec4f SELECTED_COLOUR(0.2, 1.0, 0.5, 1.0);


#endif // STYLES_H
