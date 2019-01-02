#pragma once

#include <string>

class PropertyHolder{
    public:
        PropertyHolder(std::string name, bool mandatory = false);
        virtual void setValue(std::string valueString) = 0;
        bool isMandatory();
        bool isValueGiven();
        std::string getName();
    
    protected:
        bool valueGiven;

    private:
        bool mandatory;
        std::string name;
};