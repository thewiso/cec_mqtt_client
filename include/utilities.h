#pragma once

#include <string>
#include <map>
#include <libcec/cectypes.h>

class Utilities{
    public:
        static const std::string TRUE_STRING_LITERAL;
        static const std::string FALSE_STRING_LITERAL;
        static const std::string INVALID_STRING_LITERAL;

        static const std::string GENERAL_LOGGER_NAME;
        static const std::string CEC_LOGGER_NAME;
        static const std::string MQTT_LOGGER_NAME;

        static void ToLower(std::string &string);
        static void ToUpper(std::string &string);
};