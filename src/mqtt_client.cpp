#include "mqtt_client.h"

#include "utilities.h"
#include "spdlog/spdlog.h"

#include <stdexcept>
#include <thread>
#include <chrono>

const int MqttClient::CONNECT_TIME_OUT_MILLISECONDS = 3000;
const int MqttClient::MAX_CONNECT_ATTEMPTS = 3;

MqttClient::MqttClient(const CecMqttClientProperties &properties, CecMqttClientModel *model){
    this->properties = properties;
    this->logger = spdlog::get(Utilities::MQTT_LOGGER_NAME);
    this->firstConnect = true;
    
    this->client = new mqtt::async_client(this->properties.getMqttBrokerAdress(), this->properties.getMqttClientId());

    this->mqttClientCallback = new MqttClientCallback(*client);
    mqttClientCallback->addNodeForSubscription(model->getGeneralModel()->getClientOSDNameCommand());
    mqttClientCallback->addNodeForSubscription(model->getGeneralModel()->getActiveSourceLogicalAddressCommand());
    this->client->set_callback(*mqttClientCallback);

    this->model = model;
    model->registerChangeHandler(std::bind( &MqttClient::modelNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);
     //TODO: QOS
    connectionOptions.set_keep_alive_interval(20);
    connectionOptions.set_clean_session(true);
    connectionOptions.set_automatic_reconnect(true);
}

MqttClient::~MqttClient(){
    if(client->is_connected()){
        client->disconnect()->wait();
    }
    delete client;
}

void MqttClient::connect(){
    if(!client->is_connected()){
        int connectAttempt = 0;
        while(connectAttempt<MAX_CONNECT_ATTEMPTS && !client->is_connected()){
            if(connectAttempt > 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(this->CONNECT_TIME_OUT_MILLISECONDS));
            }
            connectAttempt++;
            logger.get()->info("Connecting to MQTT server (attempt {} of {})...", connectAttempt, MAX_CONNECT_ATTEMPTS);
            
            try{
                if(firstConnect){
                    client->connect(connectionOptions);
                    firstConnect = false;
                }else{
                    client->reconnect();
                }
            }catch(const mqtt::exception& ex){
                logger.get()->warn("Exception during connecting: {}", ex.what());
            }
        }

        if(!client->is_connected()){
            throw std::runtime_error("Connecting to MQTT server failed after " + std::to_string(connectAttempt) + " attempts");
        }
    }
}

void MqttClient::modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType){
    if(modelNode.isValueNode()){
        publish(modelNode.getMqttPath(), modelNode.getValue());
    }
}

void MqttClient::publish(std::string topic, std::string value){
    try{
        client->publish(topic, value.c_str(), strlen(value.c_str()), 0, false);
    }catch (const mqtt::exception &ex) {
        logger.get()->error("Could not connect publish '{}' = '{}'. Reason: {}", topic, value, ex.get_message());
    }

   
}