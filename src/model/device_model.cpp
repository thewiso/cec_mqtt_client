#include "device_model.h"

DeviceModel::DeviceModel(const std::string &mqttPathSegment) : ModelNode(mqttPathSegment)
{
    this->name = new ModelNode("name", true);
    this->isActive = new ModelNode("isActive", true);
    this->powerStatus = new ModelNode("powerStatus", true);
    this->logicalAddress = new ModelNode("logicalAddress", true);

    powerStatusCommand = new ModelNode("powerStatus", true, std::string(), true, true);
    this->commands = new ModelNode("COMMANDS");
    this->commands->addChild(powerStatusCommand);

    addChild(name);
    addChild(isActive);
    addChild(powerStatus);
    addChild(logicalAddress);
    addChild(commands);
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

ModelNode *DeviceModel::getPowerStatusCommand(){
    return powerStatusCommand;
}