#pragma once

#include "model_node.h"

class GeneralModel: public ModelNode{
    public:
        GeneralModel();
        ModelNode *getActiveSourceLogicalAddress();

        ModelNode *getActiveSourceLogicalAddressCommand();
        ModelNode *getOSDNameCommand();

    private:
        ModelNode *activeSourceLogicalAddress;
        ModelNode *commands;
        ModelNode *OSDNameCommand;
        ModelNode *activeSourceLogicalAddressCommand;
};