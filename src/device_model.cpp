#include "device_model.h"

DeviceModel::DeviceModel(std::string deviceId) : ModelNode(deviceId)
{
    this->id = new ModelNode("id", true);
    this->name = new ModelNode("name", true);
    this->activeState = new ModelNode("activeState", true);
    this->powerState = new ModelNode("powerState", true);
    this->type = new ModelNode("type", true);

    addChild(id);
    addChild(name);
    addChild(activeState);
    addChild(powerState);
    addChild(type);
}

ModelNode *DeviceModel::getId(){
    return id;
}

ModelNode *DeviceModel::getName(){
    return name;
}

ModelNode *DeviceModel::getActiveState(){
    return activeState;
}

ModelNode *DeviceModel::getPowerState(){
    return powerState;
}

ModelNode *DeviceModel::getType(){
    return type;
}