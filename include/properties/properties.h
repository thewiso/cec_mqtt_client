#pragma once

#include <list>
#include <string>
#include <fstream>
#include "property_holder.h"

class Properties{
    typedef std::list<PropertyHolder*> PropertyHolderList;

    public:
        void readFile(const std::string &filePath);

    protected:
        void parseFile(std::ifstream &fileStream);
        virtual void fillPropertyMap() = 0;
        void checkMandatoryProperties();
        

        PropertyHolderList propertyHolders;
};