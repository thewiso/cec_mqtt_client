#pragma once

#include "properties.h"
#include <string>
// #include <stdint>

class CecMqttClientProperties
 : public Properties{
    public:
        CecMqttClientProperties();

        std::string getMqttClientId();
        std::string getMqttBrokerAdress();
        std::string getMqttTopicPrefix();

    protected:
        virtual void fillPropertyMap();
        
        std::string mqttBrokerAdress;
        std::string mqttClientId;
        std::string mqttTopicPrefix;
        
};