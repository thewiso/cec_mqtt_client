#pragma once

#include <string>

class PropertyHolder{
    public:
        PropertyHolder(const std::string &name, bool mandatory = false);
        virtual void setValue(const std::string &valueString) = 0;
        bool isMandatory();
        bool isValueGiven();
        const std::string &getName();
    
    protected:
        bool valueGiven;

    private:
        bool mandatory;
        std::string name;
};