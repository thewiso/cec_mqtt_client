#pragma once

#include "properties.h"
#include "string_property_holder.h"
#include "int_property_holder.h"
#include "bool_property_holder.h"
#include "log_level_property_holder.h"

class CecMqttClientProperties
 : public Properties{
    public:
        CecMqttClientProperties();

        const std::string &getMqttClientId();
        const std::string &getMqttBrokerAdress();
        const std::string &getMqttTopicPrefix();
        bool getLoggerLogToConsole();
        SpdLogLevel getLoggerLevel();

    protected:
        virtual void fillPropertyMap();
        
        std::string mqttBrokerAdress;
        std::string mqttClientId;
        std::string mqttTopicPrefix;

        bool loggerLogToConsole;
        SpdLogLevel loggerLevel;
};