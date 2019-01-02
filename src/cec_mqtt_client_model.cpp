#include "cec_mqtt_client_model.h"

CecMqttClientModel::CecMqttClientModel(std::string mqttRootPathSegment): ModelNode(mqttRootPathSegment){
    // commandNode = new ModelNode("commands");
    // generalNode = new ModelNode("general");
    // devicesNode = new ModelNode("devices");
    generalModel = new GeneralModel();

    // addChild(commandNode);
    // addChild(generalNode);
    // addChild(devicesNode);
    addChild(generalModel);

}

// ModelNode *CecMqttClientModel::getCommandNode(){
//     return commandNode;
// }

GeneralModel *CecMqttClientModel::getGeneralModel(){
    return generalModel;
}

// ModelNode *CecMqttClientModel::getDevicesNode(){
//     return devicesNode;
// }