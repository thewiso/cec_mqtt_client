#pragma once

#include "model_node.h"
#include "general_model.h"
#include "device_model.h"
#include <vector>
#include <utility> 


class CecMqttClientModel: public ModelNode{
    typedef std::shared_ptr<DeviceModel> DeviceModelPointer;

    public: 
        CecMqttClientModel(const std::string &mqttRootPathSegment);
        virtual void init();
        
        GeneralModel &getGeneralModel();
        const std::vector<std::shared_ptr<DeviceModel>> &getDeviceModels();
        std::pair<DeviceModel&, bool> getOrCreateDeviceModel(const std::string &logicalAddress);

    private:
        std::vector<DeviceModelPointer> deviceModels;
        std::shared_ptr<ModelNode> deviceParentNode;
        std::shared_ptr<GeneralModel> generalModel;
};