#pragma once

#include "model_node.h"

class DeviceModel: public ModelNode{
    typedef std::shared_ptr<ModelNode> ModelNodePointer;
    
    public:
        DeviceModel(const std::string &mqttPathSegment);
        virtual void init();
        
        ModelNode &getName();
        ModelNode &getIsActive();
        ModelNode &getPowerStatus();
        ModelNode &getLogicalAddress();

        ModelNode &getPowerStatusCommand();
        
    private:
        ModelNodePointer name;
        ModelNodePointer isActive;
        ModelNodePointer powerStatus;
        ModelNodePointer logicalAddress;
        
        ModelNodePointer powerStatusCommand;
};