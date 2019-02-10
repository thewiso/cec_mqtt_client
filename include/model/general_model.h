#pragma once

#include "model_node.h"

class GeneralModel: public ModelNode{
    public:
        GeneralModel();

        //TODO: return references
        ModelNode *getActiveSourceLogicalAddress();

        ModelNode *getClientOSDNameCommand();

    private:
        ModelNode *activeSourceLogicalAddress;
        ModelNode *commands;
        ModelNode *clientOSDNameCommand;
};