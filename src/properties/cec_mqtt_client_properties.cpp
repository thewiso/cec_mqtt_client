#include "cec_mqtt_client_properties.h"

CecMqttClientProperties::CecMqttClientProperties() : Properties(){
    //declare defaults
    mqttClientId = "TV_CEC_CLIENT";
    mqttTopicPrefix = "livingRoom/devices/TV";
    cecDeviceName = "MqttCecClient";
    loggerLogToConsole = false;  
    loggerLevel = SpdLogLevel::info;
}

void CecMqttClientProperties::fillPropertyMap(){
    propertyHolders.push_back(new StringPropertyHolder("MQTT_CLIENT_ID", &mqttClientId));
    propertyHolders.push_back(new StringPropertyHolder("MQTT_TOPIC_PREFIX", &mqttTopicPrefix));
    propertyHolders.push_back(new StringPropertyHolder("MQTT_BROKER_ADRESS", &mqttBrokerAdress, true));

    propertyHolders.push_back(new StringPropertyHolder("CEC_DEVICE_NAME", &cecDeviceName, true));

    propertyHolders.push_back(new BoolPropertyHolder("LOGGER_LOG_TO_CONSOLE", &loggerLogToConsole));
    propertyHolders.push_back(new LogLevelPropertyHolder("LOGGER_LEVEL", &loggerLevel));
}

const std::string &CecMqttClientProperties::getMqttClientId(){
    return mqttClientId;
}

const std::string &CecMqttClientProperties::getMqttBrokerAdress(){
    return mqttBrokerAdress;
}

const std::string &CecMqttClientProperties::getMqttTopicPrefix(){
    return mqttTopicPrefix;
}

bool CecMqttClientProperties::getLoggerLogToConsole(){
    return loggerLogToConsole;
}

SpdLogLevel CecMqttClientProperties::getLoggerLevel(){
    return loggerLevel;
}

const std::string &CecMqttClientProperties::getCecDeviceName(){
    return cecDeviceName;
}
