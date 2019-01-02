#include "properties.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

void Properties::readFile(std::string filePath){
    std::ifstream fileStream;
    fileStream.open(filePath);
    if(fileStream.is_open()){
        fillPropertyMap();
        parseFile(fileStream);
        checkMandatoryProperties();
    }else{
        throw std::runtime_error("PropertyFile " + filePath + " not found");
    }
}

void Properties::parseFile(std::ifstream &fileStream){
    
    //TODO: handle not filled properties
    std::string currentLine;
    while (std::getline(fileStream, currentLine))
    {
        std::istringstream is_line(currentLine);
        std::string key;
        if (std::getline(is_line, key, '='))
        {
            PropertyHolderList::iterator iterator = std::find_if(propertyHolders.begin(), propertyHolders.end(), [key] (PropertyHolder *propertyHolder) { return propertyHolder->getName() == key; } ); 
            // propertyHolders.find(key);
            //if map contains key && line is no comment
            if (iterator != propertyHolders.end() && key[0] != '#'){
                std::string value;
            
                if (std::getline(is_line, value))
                {
                    (*iterator)->setValue(value);
                }
            }
        }
    }

    
}

void Properties::checkMandatoryProperties(){
    for(PropertyHolderList::iterator it = propertyHolders.begin(); it != propertyHolders.end(); ++it) {
        if((*it)->isMandatory() && !(*it)->isValueGiven()){
            throw std::runtime_error("Mandatory property '" + (*it)->getName() + "' not defined in property file");
        }
    }
}