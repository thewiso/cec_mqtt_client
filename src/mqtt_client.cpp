#include "mqtt_client.h"

#include "utilities.h"
#include "spdlog/spdlog.h"

#include <stdexcept>
#include <thread>
#include <chrono>

const int MqttClient::CONNECT_TIME_OUT_MILLISECONDS = 3000;
const int MqttClient::MAX_CONNECT_ATTEMPTS = 3;
const int MqttClient::DEFAULT_QOS = 0;

MqttClient::MqttClient(const CecMqttClientProperties &properties, std::shared_ptr<CecMqttClientModel> model){
    this->properties = properties;
    this->logger = spdlog::get(Utilities::MQTT_LOGGER_NAME);
    this->firstConnect = true;
    
    this->client = std::make_unique<mqtt::async_client>(this->properties.getMqttBrokerAdress(), this->properties.getMqttClientId());

    this->mqttClientCallback = std::make_unique<MqttClientCallback>(*client);
    this->client->set_callback(*mqttClientCallback);
    
    this->model = model;
    model->registerChangeHandler(std::bind( &MqttClient::modelNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);

    connectionOptions.set_keep_alive_interval(20);
    connectionOptions.set_clean_session(true);
    connectionOptions.set_automatic_reconnect(true);
}

MqttClient::~MqttClient(){
    if(client->is_connected()){
        client->disconnect()->wait();
    }
}

void MqttClient::connect(){
    if(!client->is_connected()){
        int connectAttempt = 0;
        while(connectAttempt<MAX_CONNECT_ATTEMPTS && !client->is_connected()){
            if(connectAttempt > 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(this->CONNECT_TIME_OUT_MILLISECONDS));
            }
            connectAttempt++;
            logger->info("Connecting to MQTT server (attempt {} of {})...", connectAttempt, MAX_CONNECT_ATTEMPTS);
            
            try{
                if(firstConnect){
                    client->connect(connectionOptions)->wait();
                    firstConnect = false;
                }else{
                    client->reconnect()->wait();
                }
            }catch(const mqtt::exception& ex){
                logger->warn("Exception during connecting: {}", ex.what());
            }
        }

        if(!client->is_connected()){
            throw std::runtime_error("Connecting to MQTT server failed after " + std::to_string(connectAttempt) + " attempts");
        }
    }
}

void MqttClient::modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType){
    if(modelNodeChangeEventType == ModelNodeChangeEventType::INSERT && modelNode.isSubscriptionNode()){
        mqttClientCallback->addNodeForSubscription(modelNode);
    }
    if(modelNode.isValueNode()){
        publish(modelNode.getMqttPath(), modelNode.getValue());
    }
}

void MqttClient::publish(std::string topic, std::string value){
    try{
        logger->trace("Publishing value '{}' for topic '{}'", value, topic);
        client->publish(topic, value.c_str(), strlen(value.c_str()), MqttClient::DEFAULT_QOS, false);
    }catch (const mqtt::exception &ex) {
        logger->error("Could not connect publish '{}' = '{}'. Reason: {}", topic, value, ex.get_message());
    }

   
}