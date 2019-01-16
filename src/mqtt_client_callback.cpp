#include "mqtt_client_callback.h"
#include "spdlog/spdlog.h"
#include "utilities.h"

#include <exception>

MqttClientCallback::MqttClientCallback(mqtt::async_client &client): client(client){
    this->logger = spdlog::get(Utilities::MQTT_LOGGER_NAME);
}

void MqttClientCallback::connected(const std::string &cause){
    logger.get()->info("Connecting to MQTT server succeded.");

    for(auto it = subscriptionNodes.begin(); it != subscriptionNodes.end(); ++it){
        //TODO QOS = 1?
        logger.get()->debug("(Re)Subscribing to mqtt topic '{}'", (*it)->getMqttPath());
        try{
            client.subscribe((*it)->getMqttPath(), 1, nullptr, *this);
        }catch(const std::exception &e){
            logger.get()->error("Exception occured while subscribing to mqtt topic: {}", e.what());
        }
    }
}

//callback functions:

void MqttClientCallback::connection_lost(const std::string &cause){
    logger.get()->warn("Lost connection to broker");
}

void MqttClientCallback::message_arrived(mqtt::const_message_ptr message){
    logger.get()->trace("Message of subscribed topic '{}' arrived: '{}'", message->get_topic(), message->get_payload_str());
}

void MqttClientCallback::delivery_complete(mqtt::delivery_token_ptr token){
    logger.get()->error("Published message for topic '{}' is delivered: '{}'", token->get_message()->get_topic(), token->get_message()->get_payload_str());
    
}

//iactionlistener functions

void MqttClientCallback::on_failure(const mqtt::token &token) {
    mqtt::const_string_collection_ptr topics = token.get_topics();
    for(int i = 0; i < (*topics).size(); i++){
        logger.get()->error("Subscribtion for topic '{}' failed: {}", (*topics)[i], token.get_message_id());
    }
}

void MqttClientCallback::on_success(const mqtt::token &token) {
    mqtt::const_string_collection_ptr topics = token.get_topics();
    for(int i = 0; i < (*topics).size(); i++){
        logger.get()->debug("Subscribtion for topic '{}' succeeded.", (*topics)[i]);
    }
}

void MqttClientCallback::addNodeForSubscription(ModelNode *node){
    subscriptionNodes.push_back(node);
}