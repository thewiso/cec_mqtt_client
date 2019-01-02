#pragma once

#include <string>
#include "property_holder.h"

class StringPropertyHolder: public PropertyHolder{
    public:
        StringPropertyHolder(std::string name, std::string *value, bool mandatory = false);
        virtual void setValue(std::string valueString);
    
    private:
        std::string *value;
        

};