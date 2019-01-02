#pragma once

#include <stdexcept>
#include <string>
#include "property_holder.h"
#include <exception>

template<class T> class GenericPropertyHolder: public PropertyHolder{
    public:
        GenericPropertyHolder(std::string name, T *value, bool mandatory = false): PropertyHolder(name, mandatory){
            this->value = value;
        }

        virtual void setValue(std::string valueString){
            try { 
                *value = parseValue(valueString);
                valueGiven = true;
            } catch (const std::exception& e) {
                throw std::runtime_error("Exception occured while parsing property " + getName() + ": " + e.what());
            }
        }
        
    protected:
        virtual T parseValue(std::string valueString) = 0;
        
    private:
        T *value;
        

};

// #include "../src/generic_property_holder.cpp"