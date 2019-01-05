#pragma once

#include "model_node.h"

class DeviceModel: public ModelNode{

    public:
        DeviceModel(std::string mqttPathSegment);
        ModelNode *getName();
        ModelNode *getIsActive();
        ModelNode *getPowerStatus();
        ModelNode *getLogicalAddress();
        
    private:
        ModelNode *name;
        ModelNode *isActive;
        ModelNode *powerStatus;
        ModelNode *logicalAddress;
};