#include "device_model.h"

DeviceModel::DeviceModel(const std::string &mqttPathSegment) : ModelNode(mqttPathSegment){}

void DeviceModel::init(){
    this->name = std::make_shared<ModelNode>("name", true);
    this->isActive = std::make_shared<ModelNode>("isActive", true);
    this->powerStatus = std::make_shared<ModelNode>("powerStatus", true);
    this->logicalAddress = std::make_shared<ModelNode>("logicalAddress", true);
    this->powerStatusCommand = std::make_shared<ModelNode>("powerStatus", true, std::string(), true, true);
    ModelNodePointer commands = std::make_shared<ModelNode>("COMMANDS");
    commands->addChild(powerStatusCommand);

    addChild(name);
    addChild(isActive);
    addChild(powerStatus);
    addChild(logicalAddress);
    addChild(commands);

    ModelNode::init();
}

ModelNode &DeviceModel::getName(){
    return *name;
}

ModelNode &DeviceModel::getIsActive(){
    return *isActive;
}

ModelNode &DeviceModel::getPowerStatus(){
    return *powerStatus;
}

ModelNode &DeviceModel::getLogicalAddress(){
    return *logicalAddress;
}

ModelNode &DeviceModel::getPowerStatusCommand(){
    return *powerStatusCommand;
}