#pragma once

#include <string>
#include <cec_mqtt_client_properties.h>
#include "mqtt/client.h"
#include "model_node.h"

class MqttClient{
    public:
        MqttClient(CecMqttClientProperties properties);
        ~MqttClient();
        void modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType);

    private:
        static const int MAX_CONNECT_ATTEMPTS;
        static const int CONNECT_TIME_OUT_MILLISECONDS;
        
        mqtt::client *client;
        CecMqttClientProperties properties;
        void publish(std::string topic, std::string value);
        bool connect();
};