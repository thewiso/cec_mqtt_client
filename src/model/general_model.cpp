#include "general_model.h"


GeneralModel::GeneralModel(): ModelNode("general"){
    this->activeSourceLogicalAddress = new ModelNode("activeSourceLogicalAddress", true);
    
    clientOSDNameCommand = new ModelNode("clientOSDName", true, std::string(), true, true);
    this->commands = new ModelNode("COMMANDS");
    this->commands->addChild(clientOSDNameCommand);

    addChild(commands);
    addChild(activeSourceLogicalAddress);
}

ModelNode *GeneralModel::getActiveSourceLogicalAddress(){
    return activeSourceLogicalAddress;
}

ModelNode *GeneralModel::getClientOSDNameCommand(){
    return clientOSDNameCommand;
}