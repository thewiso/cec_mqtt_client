#pragma once

#include "property_exception.h"
#include "property_holder.h"

#include <exception>
#include <string>
#include <memory>

template<class T> class GenericPropertyHolder: public PropertyHolder{
    public:
        GenericPropertyHolder(const std::string &name, std::shared_ptr<T> value, bool mandatory = false): PropertyHolder(name, mandatory){
            this->value = value;
        }

        virtual void setValue(const std::string &valueString){
            try { 
                *value = parseValue(valueString);
                valueGiven = true;
            } catch (const std::exception& e) {
                throw PropertyException("Exception occured while parsing property " + getName() + ": " + e.what());
            }
        }
        
    protected:
        virtual T parseValue(const std::string &valueString) = 0;
        
    private:
        std::shared_ptr<T> value;
        

};
