#include <iostream>
#include "cec_mqtt_client_properties.h"
#include "mqtt_client.h"
#include "cec_client.h"
#include "cec_mqtt_client_model.h"


int main(int argc, char* argv[])
{
    CecMqttClientProperties properties;
    properties.readFile("cec_mqtt_client.conf");

     MqttClient *mqttClient = new MqttClient(properties);

    CecClient cecClient(properties);
    // cecClient.scanDevices();


    CecMqttClientModel model(properties.getMqttTopicPrefix());
    model.getGeneralNode()->registerChangeHandler(std::bind( &MqttClient::modelNodeChangeHandler, mqttClient, std::placeholders::_1, std::placeholders::_2), true);
    ModelNode *activeSource = new ModelNode("activeSourceId", true, "-1");
    model.getGeneralNode()->addChild(activeSource);
   
   
    return 0;
}