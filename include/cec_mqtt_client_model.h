#pragma once

#include "model_node.h"
#include <vector>


class CecMqttClientModel: public ModelNode{
    
    public: 
        CecMqttClientModel(std::string mqttRootPathSegment);
        ModelNode *getCommandNode();
        ModelNode *getDevicesNode();
        ModelNode *getGeneralNode();

    private:
        ModelNode *commandNode;
        ModelNode *devicesNode;
        ModelNode *generalNode;
    

};