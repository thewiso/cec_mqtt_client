#pragma once

#include "model_node.h"

class DeviceModel: public ModelNode{

    public:
        DeviceModel(std::string deviceId);
        ModelNode *getId();
        ModelNode *getName();
        ModelNode *getActiveState();
        ModelNode *getPowerState();
        ModelNode *getType();
        
    private:
        ModelNode *id;
        ModelNode *name;
        ModelNode *activeState;
        ModelNode *powerState;
        ModelNode *type;
};