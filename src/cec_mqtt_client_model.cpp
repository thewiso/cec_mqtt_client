#include "cec_mqtt_client_model.h"

CecMqttClientModel::CecMqttClientModel(std::string mqttRootPathSegment): ModelNode(mqttRootPathSegment){
    commandNode = new ModelNode("commands");
    generalNode = new ModelNode("general");
    devicesNode = new ModelNode("devices");

    addChild(commandNode);
    addChild(generalNode);
    addChild(devicesNode);

}

ModelNode *CecMqttClientModel::getCommandNode(){
    return commandNode;
}

ModelNode *CecMqttClientModel::getGeneralNode(){
    return generalNode;
}

ModelNode *CecMqttClientModel::getDevicesNode(){
    return devicesNode;
}