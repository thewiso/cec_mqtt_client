#include "general_model.h"


GeneralModel::GeneralModel(): ModelNode("general"){
    this->activeSourceLogicalAddress = new ModelNode("activeSourceLogicalAddress", true);

    addChild(activeSourceLogicalAddress);
}

ModelNode *GeneralModel::getActiveSourceLogicalAddress(){
    return activeSourceLogicalAddress;
}