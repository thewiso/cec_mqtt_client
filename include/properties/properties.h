#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include "property_holder.h"

class Properties{
    typedef std::vector<std::shared_ptr<PropertyHolder>> PropertyHolderList;

    public:
        void readFile(const std::string &filePath);

    protected:
        void parseFile(std::ifstream &fileStream);
        virtual void fillPropertyMap() = 0;
        void checkMandatoryProperties();
        

        PropertyHolderList propertyHolders;
};