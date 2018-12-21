#include "properties.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

void Properties::readFile(std::string filePath){
    std::ifstream fileStream;
    fileStream.open(filePath);
    if(fileStream.is_open()){
        parseFile(fileStream);
    }else{
        throw std::runtime_error("PropertyFile " + filePath + " not found");
    }
}

void Properties::parseFile(std::ifstream &fileStream){
    fillPropertyMap();
    //TODO: handle not filled properties
    std::string currentLine;
    while (std::getline(fileStream, currentLine))
    {
        std::istringstream is_line(currentLine);
        std::string key;
        if (std::getline(is_line, key, '='))
        {
            String2StringPointerMap::iterator iterator = propertyKey2Value.find(key);
            //if map contains key && line is no comment
            if (iterator != propertyKey2Value.end() && key[0] != '#'){
                std::string value;
            
                if (std::getline(is_line, value))
                {
                    iterator->second->assign(value);
                }
            }
        }
    }
}