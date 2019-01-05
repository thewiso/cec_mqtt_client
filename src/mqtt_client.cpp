#include "mqtt_client.h"

#include <thread>
#include <chrono>

const int MqttClient::CONNECT_TIME_OUT_MILLISECONDS = 3000;
const int MqttClient::MAX_CONNECT_ATTEMPTS = 3;

MqttClient::MqttClient(const CecMqttClientProperties &properties, CecMqttClientModel *model, const std::shared_ptr<spdlog::logger> &logger){
    this->properties = properties;
    this->model = model;
    this->client = new mqtt::client(this->properties.getMqttBrokerAdress(), this->properties.getMqttClientId());
    this->logger = logger;
    firstConnect = true;

    model->registerChangeHandler(std::bind( &MqttClient::modelNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);
     //TODO: QOS
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);
    
}

MqttClient::~MqttClient(){
    if(client->is_connected()){
        client->disconnect();
    }
    delete client;
}

bool MqttClient::connect(){
    if(!client->is_connected()){
        int connectAttempt = 0;
        while(connectAttempt<MAX_CONNECT_ATTEMPTS && !client->is_connected()){
            if(connectAttempt > 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(this->CONNECT_TIME_OUT_MILLISECONDS));
            }
            connectAttempt++;
            logger.get()->info("Connecting to MQTT server (attempt {} of {})...", connectAttempt, MAX_CONNECT_ATTEMPTS);
            
            if(firstConnect){
                client->connect(connOpts);
                firstConnect = false;
            }else{
                client->reconnect();

            }
        }
        bool retVal =  client->is_connected();
        logger.get()->info("Connecting to MQTT server {}", (retVal ? "succeded." : "failed"));
        return retVal;
    }else{
        return true;
    }

}

void MqttClient::modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType){
    if(modelNode.isValueNode() && modelNodeChangeType == ModelNodeChangeType::UPDATE){
        publish(modelNode.getMqttPath(), modelNode.getValue());
    }
}

void MqttClient::publish(std::string topic, std::string value){
    if(connect()){
        try {
            client->publish(topic, value.c_str(), strlen(value.c_str()), 0, false);
        }
        //TODO: better error handling
        catch (const mqtt::exception& exc) {
            std::cerr << "Error: " << exc.what() << " ["
                << exc.get_reason_code() << "]" << std::endl;
        }
    }else{
        logger.get()->error("Could not connect to MQTT broker to publish '{}' = '{}'", topic, value);
    } 

   
}