#include "general_model.h"


GeneralModel::GeneralModel(): ModelNode("general"){}

void GeneralModel::init(){
    this->activeSourceLogicalAddress =  std::make_shared<ModelNode>("activeSourceLogicalAddress", true);
    this->clientOSDNameCommand =  std::make_shared<ModelNode>("clientOSDName", true, std::string(), true, true);
    ModelNodePointer commands =  std::make_shared<ModelNode>("COMMANDS");
    commands->addChild(clientOSDNameCommand);
    
    addChild(commands);
    addChild(activeSourceLogicalAddress);

    ModelNode::init();
}

ModelNode &GeneralModel::getActiveSourceLogicalAddress(){
    return *activeSourceLogicalAddress;
}

ModelNode &GeneralModel::getClientOSDNameCommand(){
    return *clientOSDNameCommand;
}