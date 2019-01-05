#include "device_model.h"

DeviceModel::DeviceModel(std::string mqttPathSegment) : ModelNode(mqttPathSegment)
{
    this->name = new ModelNode("name", true);
    this->isActive = new ModelNode("isActive", true);
    this->powerStatus = new ModelNode("powerStatus", true);
    this->logicalAddress = new ModelNode("logicalAddress", true);

    addChild(name);
    addChild(isActive);
    addChild(powerStatus);
    addChild(logicalAddress);
}

ModelNode *DeviceModel::getName(){
    return name;
}

ModelNode *DeviceModel::getIsActive(){
    return isActive;
}

ModelNode *DeviceModel::getPowerStatus(){
    return powerStatus;
}

ModelNode *DeviceModel::getLogicalAddress(){
    return logicalAddress;
}