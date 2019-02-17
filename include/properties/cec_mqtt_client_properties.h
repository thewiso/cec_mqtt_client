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
        const std::string &getCecDeviceName();
        bool getLoggerLogToConsole();
        SpdLogLevel getLoggerLevel();

    protected:
        virtual void fillPropertyMap();
        
        std::shared_ptr<std::string> mqttBrokerAdress;
        std::shared_ptr<std::string> mqttClientId;
        std::shared_ptr<std::string> mqttTopicPrefix;

        std::shared_ptr<std::string> cecDeviceName;

        std::shared_ptr<bool> loggerLogToConsole;
        std::shared_ptr<SpdLogLevel> loggerLevel;
};