#include "general_model.h"


GeneralModel::GeneralModel(): ModelNode("general"){
    this->activeSourceLogicalAddress = new ModelNode("activeSourceLogicalAddress", true);
    
    activeSourceLogicalAddressCommand = new ModelNode("activeSourceLogicalAddress", true);
    OSDNameCommand = new ModelNode("OSDName", true);
    this->commands = new ModelNode("COMMANDS");
    this->commands->addChild(activeSourceLogicalAddressCommand, true);
    this->commands->addChild(OSDNameCommand, true);

    addChild(commands);
    addChild(activeSourceLogicalAddress);
}

ModelNode *GeneralModel::getActiveSourceLogicalAddress(){
    return activeSourceLogicalAddress;
}

ModelNode *GeneralModel::getActiveSourceLogicalAddressCommand(){
    return activeSourceLogicalAddressCommand;
}

ModelNode *GeneralModel::getOSDNameCommand(){
    return OSDNameCommand;
}