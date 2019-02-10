#include "utilities.h"

#include <algorithm>
#include <cctype>

const std::string Utilities::TRUE_STRING_LITERAL = "true";
const std::string Utilities::FALSE_STRING_LITERAL = "false";
const std::string Utilities::INVALID_STRING_LITERAL = "-1";

const std::string Utilities::GENERAL_LOGGER_NAME = "GENERAL";
const std::string Utilities::CEC_LOGGER_NAME = "CEC";
const std::string Utilities::MQTT_LOGGER_NAME = "MQTT";

void Utilities::ToLower(std::string &string){
    std::transform(string.begin(), string.end(), string.begin(), ::tolower);
}

void Utilities::ToUpper(std::string &string){
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);
}