#ifndef MACROS_H
#define MACROS_H

#include <iostream>

//Enable debugging
#define DEBUG
//#define TEXT_ONLY

#ifdef DEBUG
#define TRACE(arg) (std::cout << arg << std::endl)
#else
#define TRACE(arg) sizeof(std::cout << arg << std::endl)
#endif

//a simple macro to display the symbolic name of enums
#define NAME_OF(x) #x

#endif // MACROS_H
