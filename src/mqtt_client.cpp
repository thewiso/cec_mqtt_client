#include "mqtt_client.h"

#include <thread>
#include <chrono>

const int MqttClient::CONNECT_TIME_OUT_MILLISECONDS = 3000;
const int MqttClient::MAX_CONNECT_ATTEMPTS = 3;

MqttClient::MqttClient(CecMqttClientProperties properties){
    this->properties = properties;
    client = new mqtt::client(properties.getMqttBrokerAdress(), properties.getMqttClientId());

    //TODO: QOS
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);
    client->connect(connOpts);
}

MqttClient::~MqttClient(){
    if(client->is_connected()){
        client->disconnect();
    }
    delete client;
}

bool MqttClient::connect(){
    if(!client->is_connected()){
        //TODO: in extra thread to prevent blocking of main thread
        int connectAttempt = 0;
        while(connectAttempt<MAX_CONNECT_ATTEMPTS && !client->is_connected()){
            if(connectAttempt > 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(this->CONNECT_TIME_OUT_MILLISECONDS));
            }

            connectAttempt++;
            //TODO: log level
            std::cout << "Reconnecting to MQTT server (attempt " << connectAttempt << " of " << MAX_CONNECT_ATTEMPTS << ")...\n";
            client->reconnect();
        }

        return client->is_connected();
    }else{
        return true;
    }

}

void MqttClient::modelNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType){
    publish(modelNode.getMqttPath(), modelNode.getValue());
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
        std::cerr << "Could not connect to MQTT broker to publish " << topic << ":" << value;
    } 

   
}