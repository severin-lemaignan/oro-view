#ifndef OROVIEW_EXCEPTIONS_H
#define OROVIEW_EXCEPTIONS_H

#include <stdexcept>

class OroViewException : public std::runtime_error {
	public:
		OroViewException() : std::runtime_error("A generic exception has been thrown in oro view.") { }
		OroViewException(const char* msg) : std::runtime_error(msg) { }
		OroViewException(const std::string& msg) : std::runtime_error(msg.c_str()) { }
};

#endif // OROVIEW_EXCEPTIONS_H
