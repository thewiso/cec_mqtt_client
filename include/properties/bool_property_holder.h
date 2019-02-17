#pragma once

#include "generic_property_holder.h"

class BoolPropertyHolder: public GenericPropertyHolder<bool>{
     public:
        BoolPropertyHolder(const std::string &name, std::shared_ptr<bool> value, bool mandatory = false);
        
    protected:
        virtual bool parseValue(const std::string &valueString);
};