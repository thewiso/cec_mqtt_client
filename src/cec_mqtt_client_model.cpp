#include "cec_mqtt_client_model.h"

#include <algorithm>

CecMqttClientModel::CecMqttClientModel(std::string mqttRootPathSegment): ModelNode(mqttRootPathSegment){
    generalModel = new GeneralModel();
    deviceParentNode = new ModelNode("devices");

    addChild(generalModel);
    addChild(deviceParentNode);
}

GeneralModel *CecMqttClientModel::getGeneralModel(){
    return generalModel;
}

std::vector<DeviceModel*> CecMqttClientModel::getDeviceModels(){
    return deviceModels;
}

#include <iostream>
std::pair<DeviceModel*, bool> CecMqttClientModel::getOrCreateDeviceModel(std::string logicalAddress){
    std::vector<DeviceModel*>::iterator iterator = std::find_if(deviceModels.begin(), deviceModels.end(), [logicalAddress] (DeviceModel *deviceModel) { return deviceModel->getLogicalAddress()->getValue() == logicalAddress; } ); 

    bool createDevice = iterator == deviceModels.end();
    
    std::pair<DeviceModel*, bool> retVal;    
    retVal.second = createDevice;

    if (createDevice){
        DeviceModel *newModel  = new DeviceModel(logicalAddress);
        deviceParentNode->addChild(newModel);
        deviceModels.push_back(newModel);
        newModel->getLogicalAddress()->setValue(logicalAddress);
        retVal.first = newModel;
    }else{
        retVal.first = (*iterator);
    }

    return retVal;
}
