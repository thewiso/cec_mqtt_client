#pragma once

#include <stdexcept>

class PropertyException: public std::runtime_error{
    public:
        PropertyException(const char *what) : runtime_error(what){};
        PropertyException(const std::string &what) : runtime_error(what){};
};