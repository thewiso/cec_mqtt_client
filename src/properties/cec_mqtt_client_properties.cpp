#include "cec_mqtt_client_properties.h"

CecMqttClientProperties::CecMqttClientProperties() : Properties(){
    mqttBrokerAdress = std::make_shared<std::string>();
    mqttClientId = std::make_shared<std::string>("TV_CEC_CLIENT");
    mqttTopicPrefix = std::make_shared<std::string>("livingRoom/devices/TV");
    cecDeviceName = std::make_shared<std::string>("MqttCecClient");
    loggerLogToConsole = std::make_shared<bool>(false);  
    loggerLevel = std::make_shared<SpdLogLevel>(SpdLogLevel::info);
}

void CecMqttClientProperties::fillPropertyMap(){
    propertyHolders.push_back(std::make_shared<StringPropertyHolder>("MQTT_CLIENT_ID", mqttClientId));
    propertyHolders.push_back(std::make_shared<StringPropertyHolder>("MQTT_TOPIC_PREFIX", mqttTopicPrefix));
    propertyHolders.push_back(std::make_shared<StringPropertyHolder>("MQTT_BROKER_ADRESS", mqttBrokerAdress, true));

    propertyHolders.push_back(std::make_shared<StringPropertyHolder>("CEC_DEVICE_NAME", cecDeviceName));

    propertyHolders.push_back(std::make_shared<BoolPropertyHolder>("LOGGER_LOG_TO_CONSOLE", loggerLogToConsole));
    propertyHolders.push_back(std::make_shared<LogLevelPropertyHolder>("LOGGER_LEVEL", loggerLevel));
}

const std::string &CecMqttClientProperties::getMqttClientId(){
    return *mqttClientId;
}

const std::string &CecMqttClientProperties::getMqttBrokerAdress(){
    return *mqttBrokerAdress;
}

const std::string &CecMqttClientProperties::getMqttTopicPrefix(){
    return *mqttTopicPrefix;
}

bool CecMqttClientProperties::getLoggerLogToConsole(){
    return *loggerLogToConsole;
}

SpdLogLevel CecMqttClientProperties::getLoggerLevel(){
    return *loggerLevel;
}

const std::string &CecMqttClientProperties::getCecDeviceName(){
    return *cecDeviceName;
}
