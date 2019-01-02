#pragma once

#include "generic_property_holder.h"

class IntPropertyHolder: public GenericPropertyHolder<int>{
     public:
        IntPropertyHolder(std::string name, int *value, bool mandatory = false);
        
    protected:
        virtual int parseValue(std::string valueString);
};