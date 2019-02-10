#pragma once

#include "model_node.h"
#include "general_model.h"
#include "device_model.h"
#include <vector>
#include <utility> 


class CecMqttClientModel: public ModelNode{
    
    public: 
        CecMqttClientModel(const std::string &mqttRootPathSegment);
        GeneralModel *getGeneralModel();
        const std::vector<DeviceModel*> &getDeviceModels();
        //TODO: return references? (may cause error)
        std::pair<DeviceModel*, bool> getOrCreateDeviceModel(const std::string &logicalAddress);

    private:
    //TODO: return references
        std::vector<DeviceModel*> deviceModels;
        ModelNode *deviceParentNode;
        GeneralModel *generalModel;
};