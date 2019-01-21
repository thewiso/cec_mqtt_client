#pragma once

#include "generic_property_holder.h"

class BoolPropertyHolder: public GenericPropertyHolder<bool>{
     public:
        BoolPropertyHolder(const std::string &name, bool *value, bool mandatory = false);
        
    protected:
        virtual bool parseValue(const std::string &valueString);
};