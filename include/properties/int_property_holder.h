#pragma once

#include "generic_property_holder.h"

class IntPropertyHolder: public GenericPropertyHolder<int>{
    public:
        IntPropertyHolder(const std::string &name, int *value, bool mandatory = false);
        
    protected:
        virtual int parseValue(const std::string &valueString);
};