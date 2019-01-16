#pragma once

#include "mqtt/async_client.h"
#include "model_node.h"

#include <memory>
#include <string>
#include <vector>



class MqttClientCallback: public mqtt::callback, public mqtt::iaction_listener{
    public:
        MqttClientCallback(mqtt::async_client &client);

        void connected(const std::string &cause) override;
        void connection_lost(const std::string &cause) override;
        void message_arrived(mqtt::const_message_ptr msg) override;
        void delivery_complete(mqtt::delivery_token_ptr token) override;

        void on_failure(const mqtt::token &token) override;
        void on_success(const mqtt::token &token) override;

        void addNodeForSubscription(ModelNode *node);

    private:
        mqtt::async_client &client;
        std::shared_ptr<spdlog::logger> logger;
        std::vector<ModelNode*> subscriptionNodes;
        
};