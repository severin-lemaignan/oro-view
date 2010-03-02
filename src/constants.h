#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include "core/vectors.h"

enum relation_type {SUBCLASS, SUPERCLASS, INSTANCE, OBJ_PROPERTY, DATA_PROPERTY, UNDEFINED};

static const std::string dateFormat("%A, %d %B, %Y %X");
static const float shadowStrength = 0.5;

static const float GRAVITY = 9.81;

static const float INITIAL_MASS = 1.0; //kg :)
static const float INITIAL_DAMPING = 0.95; // 0<damping<1. 1 means no damping at all.

static const float COULOMB_CONSTANT = 20000.0; // impact the strenght of repulsion between nodes.
static const float INITIAL_CHARGE = 1.0; // the charges of each pair of nodes ae multiplied by the Coulomb constant to compute repulsion

static const float INITIAL_SPRING_CONSTANT = 20.0; // N.pixels^(-1)
static const float NOMINAL_EDGE_LENGTH = 50.0; // pixels

static const float MIN_KINETIC_ENERGY = 1.0; //Nodes with a lower energy won't move at all.

static const vec4f HOVERED_COLOUR(1.0, 0.2, 0.5, 1.0);
static const vec4f SELECTED_COLOUR(0.2, 1.0, 0.5, 1.0);

#endif // CONSTANTS_H

