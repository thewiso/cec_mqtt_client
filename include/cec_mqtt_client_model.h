#pragma once

#include "model_node.h"
#include "general_model.h"
#include "device_model.h"
#include <vector>


class CecMqttClientModel: public ModelNode{
    
    public: 
        CecMqttClientModel(std::string mqttRootPathSegment);
        // ModelNode *getCommandNode();
        // ModelNode *getDevicesNode();
        GeneralModel *getGeneralModel();

    private:
        // ModelNode *commandNode;
        // ModelNode *devicesNode;
        GeneralModel *generalModel;
    

};