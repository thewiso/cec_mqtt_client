#pragma once

#include "model_node.h"

class GeneralModel: public ModelNode{
    typedef std::shared_ptr<ModelNode> ModelNodePointer;

    public:
        GeneralModel();
        virtual void init();

        ModelNode &getActiveSourceLogicalAddress();
        
        ModelNode &getClientOSDNameCommand();

    private:
        ModelNodePointer activeSourceLogicalAddress;
        
        ModelNodePointer clientOSDNameCommand;
};