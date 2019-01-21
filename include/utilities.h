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
        //TODO: which members belong truly in this class? 
        static const std::map<CEC::cec_logical_address, std::string> CEC_LOGICAL_ADRESS_2_STRING_LITERAL;
        static const std::map<CEC::cec_power_status, std::string> CEC_POWER_STATUS_2_STRING_LITERAL;
        
        static int parseStringToBool(std::string string);


};