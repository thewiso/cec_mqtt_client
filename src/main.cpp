#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>

#include "cec_mqtt_client_properties.h"
#include "mqtt_client.h"
#include "cec_client.h"
#include "cec_mqtt_client_model.h"

//TODO: use lambda in signal function?
std::atomic<bool> interrupt(false); 
void handle_signal(int signal)
{
    interrupt.store(true);
}


int main(int argc, char* argv[])
{
    //TODO: smart pointer!!
    //TODO: general update command
    signal(SIGINT, handle_signal);

    CecMqttClientProperties properties;
    properties.readFile("cec_mqtt_client.conf");

    CecMqttClientModel model(properties.getMqttTopicPrefix());
    
    MqttClient *mqttClient = new MqttClient(properties, model);
    CecClient *cecClient = CecClient::getInstance(properties, model);

    mqttClient->init();
    cecClient->init();

    while(!interrupt.load()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //FIXME:
    //update activeSourceDeviceId when turning tv on / off
    //update device 

    //TODO: TEST
    //toggle activeDevice
    //network disconnect mqtt
    //hdmi disconnect cec
    return 0;
}