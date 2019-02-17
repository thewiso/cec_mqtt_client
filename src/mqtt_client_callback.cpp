#include "mqtt_client_callback.h"
#include "spdlog/spdlog.h"
#include "utilities.h"
#include "mqtt_client.h"

#include <exception>
#include <algorithm>

MqttClientCallback::MqttClientCallback(mqtt::async_client &client): client(client){
    this->logger = spdlog::get(Utilities::MQTT_LOGGER_NAME);
}

void MqttClientCallback::connected(const std::string &cause){
    logger->info("Connecting to MQTT server succedeed.");

    subscriptionNodesVectorMutex.lock();
    for(auto it = subscriptionNodes.begin(); it != subscriptionNodes.end(); ++it){
        subscribeToNode(*(*it));          
    }
    subscriptionNodesVectorMutex.unlock();
}

//callback functions:

void MqttClientCallback::connection_lost(const std::string &cause){
    logger->warn("Lost connection to broker");
}

void MqttClientCallback::message_arrived(mqtt::const_message_ptr message){
    std::string topic = message->get_topic();
    logger->trace("Message of subscribed topic '{}' arrived: '{}'", topic, message->get_payload_str());
    auto iterator = std::find_if(subscriptionNodes.begin(), subscriptionNodes.end(), [topic] (ModelNode *modelNode){
        return modelNode->getMqttPath() == topic;
    });

    if(iterator != subscriptionNodes.end()){
        (*iterator)->setValue(message->get_payload_str());
    }else{
        logger->error("Could not find model node for subscribed topic {}", topic);
    }
}

void MqttClientCallback::delivery_complete(mqtt::delivery_token_ptr token){
    logger->error("Published message for topic '{}' is delivered: '{}'", token->get_message()->get_topic(), token->get_message()->get_payload_str());
    
}

//iactionlistener functions

void MqttClientCallback::on_failure(const mqtt::token &token) {
    mqtt::const_string_collection_ptr topics = token.get_topics();
    for(int i = 0; i < (*topics).size(); i++){
        logger->error("Subscribtion for topic '{}' failed: {}", (*topics)[i], token.get_message_id());
    }
}

void MqttClientCallback::on_success(const mqtt::token &token) {
    mqtt::const_string_collection_ptr topics = token.get_topics();
    for(int i = 0; i < (*topics).size(); i++){
        logger->debug("Subscribtion for topic '{}' succeeded.", (*topics)[i]);
    }
}

void MqttClientCallback::addNodeForSubscription(ModelNode &node){
    subscriptionNodes.push_back(&node);
    if(client.is_connected()){
        subscriptionNodesVectorMutex.lock();
        subscribeToNode(node);
        subscriptionNodesVectorMutex.unlock();
    }
}

void MqttClientCallback::subscribeToNode(ModelNode &node){
    try{
        std::string mqttPath = node.getMqttPath();
        logger->debug("(Re)Subscribing to mqtt topic '{}'", mqttPath);
        client.subscribe(mqttPath, MqttClient::DEFAULT_QOS, nullptr, *this);
    }catch(const std::exception &e){
        logger->error("Exception occured while subscribing to mqtt topic: {}", e.what());
    }
}