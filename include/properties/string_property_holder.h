#pragma once

#include <string>
#include <memory>
#include "property_holder.h"

class StringPropertyHolder: public PropertyHolder{
    public:
        StringPropertyHolder(const std::string &name, std::shared_ptr<std::string> value, bool mandatory = false);
        virtual void setValue(const std::string &valueString);
    
    private:
        std::shared_ptr<std::string> value;
        

};