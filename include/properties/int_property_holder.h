#pragma once

#include "generic_property_holder.h"

class IntPropertyHolder: public GenericPropertyHolder<int>{
    public:
        IntPropertyHolder(const std::string &name, std::shared_ptr<int> value, bool mandatory = false);
        
    protected:
        virtual int parseValue(const std::string &valueString);
};