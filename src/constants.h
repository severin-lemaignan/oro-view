#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

enum relation_type {SUBCLASS, SUPERCLASS, INSTANCE, OBJ_PROPERTY, DATA_PROPERTY};

static const std::string dateFormat("%A, %d %B, %Y %X");
static const float shadowStrength = 0.5;

static const float gravity = 9.81;

#endif // CONSTANTS_H

