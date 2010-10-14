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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include "core/vectors.h"

enum node_type {CLASS_NODE, INSTANCE_NODE, LITERAL_NODE, COMMENT_NODE, TRUE_NODE, FALSE_NODE};

//note that PROPERTY is either OBJ_PROPERTY or DATA_PROPERTY or COMMENT
enum relation_type {SUBCLASS, SUPERCLASS, INSTANCE, CLASS, PROPERTY, OBJ_PROPERTY, DATA_PROPERTY, COMMENT, UNDEFINED};

static const std::string dateFormat("%A, %d %B, %Y %X");

static const float GRAVITY = 9.81;

static const float INITIAL_MASS = 1.0; //kg :)
static const float INITIAL_DAMPING = 0.95; // 0<damping<1. 1 means no damping at all.

static const float COULOMB_CONSTANT = 20000.0; // impact the strenght of repulsion between nodes.
static const float INITIAL_CHARGE = 1.0; // the charges of each pair of nodes ae multiplied by the Coulomb constant to compute repulsion

static const float INITIAL_SPRING_CONSTANT = 20.0; // N.pixels^(-1)
static const float NOMINAL_EDGE_LENGTH = 50.0; // pixels

static const float MIN_KINETIC_ENERGY = 1.0; //Nodes with a lower energy won't move at all.

//static const std::string ROOT_CONCEPT = "http%3A%2F%2Fwww.w3.org%2F2002%2F07%2Fowl%23Thing"; // http://www.w3.org/2002/07/owl#Thing
//static const std::string ROOT_CONCEPT = "owl:Thing";
static const std::string ROOT_CONCEPT = "myself";

#endif // CONSTANTS_H

