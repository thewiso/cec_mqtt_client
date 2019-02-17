#pragma once

#include <map>

#include "generic_property_holder.h"
#include "spdlog/common.h"

typedef spdlog::level::level_enum SpdLogLevel;

class LogLevelPropertyHolder: public GenericPropertyHolder<spdlog::level::level_enum>{
    public:
        LogLevelPropertyHolder(const std::string &name, std::shared_ptr<SpdLogLevel> value, bool mandatory = false);
        
    protected:
        virtual SpdLogLevel parseValue(const std::string &valueString);
        static const std::map<std::string, SpdLogLevel> STRING_LITERAL_2_SPD_LOG_LEVEL;
};