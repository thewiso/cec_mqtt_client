#pragma once

#include "properties.h"
#include <string>
// #include <stdint>

class CecMqttClientProperties
 : public Properties{
    public:
        CecMqttClientProperties();

        const std::string &getMqttClientId();
        const std::string &getMqttBrokerAdress();
        const std::string &getMqttTopicPrefix();

    protected:
        virtual void fillPropertyMap();
        
        std::string mqttBrokerAdress;
        std::string mqttClientId;
        std::string mqttTopicPrefix;
        
};