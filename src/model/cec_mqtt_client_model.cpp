#include "cec_mqtt_client_model.h"

#include <algorithm>

CecMqttClientModel::CecMqttClientModel(const std::string &mqttRootPathSegment): ModelNode(mqttRootPathSegment){
   
}

void CecMqttClientModel::init(){
    this->generalModel = std::make_shared<GeneralModel>();
    this->deviceParentNode = std::make_shared<ModelNode>("devices");

    addChild(generalModel);
    addChild(deviceParentNode);

    ModelNode::init();
}

GeneralModel &CecMqttClientModel::getGeneralModel(){
    return *generalModel;
}

const std::vector<std::shared_ptr<DeviceModel>> &CecMqttClientModel::getDeviceModels(){
    return deviceModels;
}

std::pair<DeviceModel&, bool> CecMqttClientModel::getOrCreateDeviceModel(const std::string &logicalAddress){
    auto iterator = std::find_if(deviceModels.begin(), deviceModels.end(), [logicalAddress] (DeviceModelPointer &deviceModel) { return (*deviceModel).getLogicalAddress().getValue() == logicalAddress; } ); 
    bool createDevice = iterator == deviceModels.end();
    DeviceModelPointer deviceModel;
    
    if (createDevice){
        deviceModel = std::make_shared<DeviceModel>(logicalAddress);
        deviceModel->init();
        deviceParentNode->addChild(deviceModel);
        deviceModel->resumeChangeEvents();
        deviceModel->getLogicalAddress().setValue(logicalAddress);
        deviceModels.push_back(deviceModel);
    }else{
        deviceModel = *iterator;
    }
    
    std::pair<DeviceModel&, bool> retVal(*deviceModel, createDevice);    
    return retVal;
}

