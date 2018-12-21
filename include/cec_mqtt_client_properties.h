#pragma once

#include "properties.h"
#include <string>

class CecMqttClientProperties
 : public Properties{
    public:
        CecMqttClientProperties();
        std::string getClientId();

    protected:
        virtual void fillPropertyMap();
        
        std::string clientId;
        
};