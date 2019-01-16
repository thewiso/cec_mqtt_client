#pragma once

#include "model_node.h"

class GeneralModel: public ModelNode{
    public:
        GeneralModel();
        ModelNode *getActiveSourceLogicalAddress();

        ModelNode *getActiveSourceLogicalAddressCommand();
        ModelNode *getClientOSDNameCommand();

    private:
        ModelNode *activeSourceLogicalAddress;
        ModelNode *commands;
        ModelNode *clientOSDNameCommand;
        ModelNode *activeSourceLogicalAddressCommand;
};