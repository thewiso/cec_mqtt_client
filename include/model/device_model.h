#pragma once

#include "model_node.h"

class DeviceModel: public ModelNode{

    public:
        DeviceModel(const std::string &mqttPathSegment);
        //TODO: return references
        ModelNode *getName();
        ModelNode *getIsActive();
        ModelNode *getPowerStatus();
        ModelNode *getLogicalAddress();

        ModelNode *getPowerStatusCommand();
        
    private:
        ModelNode *name;
        ModelNode *isActive;
        ModelNode *powerStatus;
        ModelNode *logicalAddress;

        ModelNode *commands;
        ModelNode *powerStatusCommand;
};