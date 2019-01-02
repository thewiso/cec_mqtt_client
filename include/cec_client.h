#pragma once

#include "cec_mqtt_client_properties.h"
#include "cec_mqtt_client_model.h"

#include <libcec/cec.h>

class CecClient{
    public:
        CecClient(CecMqttClientProperties properties, CecMqttClientModel &model);
        ~CecClient();
        void init();

    private:
       CecMqttClientProperties properties;
       CecMqttClientModel *model;
       CEC::ICECAdapter *cecAdapter;
       
       void updateModel();
};