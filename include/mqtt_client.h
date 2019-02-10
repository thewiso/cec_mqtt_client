#pragma once

#include <string>
#include <memory>

#include "mqtt/async_client.h"
#include "spdlog/logger.h"

#include "cec_mqtt_client_properties.h"
#include "model_node.h"
#include "cec_mqtt_client_model.h"
#include "mqtt_client_callback.h"

class MqttClient{
    public:
        MqttClient(const CecMqttClientProperties &properties, CecMqttClientModel *model);
        ~MqttClient();
        void connect();
        
    private:
        static const int MAX_CONNECT_ATTEMPTS;
        static const int CONNECT_TIME_OUT_MILLISECONDS;
        
        mqtt::async_client *client;
        mqtt::connect_options connectionOptions;
        bool firstConnect;
        CecMqttClientProperties properties;
        CecMqttClientModel *model;
        std::shared_ptr<spdlog::logger> logger;
        MqttClientCallback *mqttClientCallback;

        void publish(std::string topic, std::string value);
        void modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType);
};