#pragma once

#include <string>
#include <memory>

#include <mqtt/client.h>
#include "spdlog/logger.h"

#include "cec_mqtt_client_properties.h"
#include "model_node.h"
#include "cec_mqtt_client_model.h"

class MqttClient{
    public:
        MqttClient(const CecMqttClientProperties &properties, CecMqttClientModel *model, const std::shared_ptr<spdlog::logger> &logger);
        ~MqttClient();
        void modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType);
        void init();
        
    private:
        static const int MAX_CONNECT_ATTEMPTS;
        static const int CONNECT_TIME_OUT_MILLISECONDS;
        
        mqtt::client *client;
        CecMqttClientProperties properties;
        CecMqttClientModel *model;
        std::shared_ptr<spdlog::logger> logger;

        void publish(std::string topic, std::string value);
        bool connect();
};