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

#ifndef MACROS_H
#define MACROS_H

#include <iostream>
#include <iomanip>

//Enable debugging
//#define DEBUG
//#define TEXT_ONLY

#ifdef DEBUG
#define TRACE(arg) (std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2) << arg << std::endl)
#else
#define TRACE(arg) sizeof(std::cout << arg << std::endl)
#endif

#endif // MACROS_H
