#ifndef STYLES_H
#define STYLES_H

#include "core/vectors.h"

enum rendering_mode {NORMAL, SIMPLE, SHADOWS, BLOOM, NAMES};

static const float FADE_TIME = 5.0; //idle time (in sec) before labels vanish

static const vec4f HOVERED_COLOUR(1.0, 0.2, 0.5, 1.0);
static const vec4f SELECTED_COLOUR(0.2, 1.0, 0.5, 1.0);


#endif // STYLES_H
