#include "general_model.h"


GeneralModel::GeneralModel(): ModelNode("general"){
    this->activeSourceDeviceId = new ModelNode("activeSourceDeviceId", true);

    addChild(activeSourceDeviceId);
}

ModelNode *GeneralModel::getActiveSourceDeviceId(){
    return activeSourceDeviceId;
}