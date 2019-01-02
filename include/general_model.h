#pragma once

#include "model_node.h"

class GeneralModel: public ModelNode{
    public:
        GeneralModel();
        ModelNode *getActiveSourceDeviceId();

    private:
        ModelNode *activeSourceDeviceId;
};