#pragma once

#include "cec_mqtt_client_properties.h"

class CecClient{
    public:
        CecClient(CecMqttClientProperties properties);
        ~CecClient();
        void scanDevices();

    private:
       CecMqttClientProperties properties;
};