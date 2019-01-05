#pragma once

#include "model_node.h"
#include "general_model.h"
#include "device_model.h"
#include <vector>
#include <utility> 


class CecMqttClientModel: public ModelNode{
    
    public: 
        CecMqttClientModel(std::string mqttRootPathSegment);
        GeneralModel *getGeneralModel();
        std::vector<DeviceModel*> getDeviceModels();
        std::pair<DeviceModel*, bool> getOrCreateDeviceModel(std::string logicalAddress);

    private:
        std::vector<DeviceModel*> deviceModels;
        ModelNode *deviceParentNode;
        GeneralModel *generalModel;
    

};