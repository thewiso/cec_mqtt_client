#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>
#include <initializer_list> 
#include <exception>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "cec_mqtt_client_properties.h"
#include "mqtt_client.h"
#include "cec_client.h"
#include "cec_mqtt_client_model.h"
#include "property_exception.h"
#include "utilities.h"


std::atomic<bool> interrupt(false); 
void handle_signal(int signal)
{
    interrupt.store(true);
}


int main(int argc, char* argv[])
{
    //TODO: log file path, size as constant
    //1024 * 1024 * 5 = 5 Mebibyte, 2 log filesspdlog::level::debug
    auto sharedFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("cec_mqtt_client_log.txt", 1024 * 1024 * 5, 2);
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    spdlog::sinks_init_list loggerSinks = {sharedFileSink, consoleSink};
    
    auto cecLogger = std::make_shared<spdlog::logger>(Utilities::CEC_LOGGER_NAME, loggerSinks);
    auto mqttLogger = std::make_shared<spdlog::logger>(Utilities::MQTT_LOGGER_NAME, loggerSinks);
    auto generalLogger = std::make_shared<spdlog::logger>(Utilities::GENERAL_LOGGER_NAME, loggerSinks);
    spdlog::register_logger(cecLogger);
    spdlog::register_logger(mqttLogger);
    spdlog::register_logger(generalLogger);

    //TODO: implement usage of auto (e.g. in for loop iterator)
    //TODO: smart pointer??
    //TODO: ptr.get() durch *ptr ersetzen!
    //TODO: general update command
    signal(SIGINT, handle_signal);

    try{
        CecMqttClientProperties properties;
        properties.readFile("cec_mqtt_client.conf");

        if(!properties.getLoggerLogToConsole()){
            (*consoleSink).set_level(spdlog::level::level_enum::off);
        }
        SpdLogLevel logLevel = properties.getLoggerLevel();
        generalLogger.get()->set_level(logLevel);
        mqttLogger.get()->set_level(logLevel);
        cecLogger.get()->set_level(logLevel);

        generalLogger.get()->info("Finished reading property file.");
        generalLogger.get()->info("Initializing client...");

        CecMqttClientModel *model = new CecMqttClientModel(properties.getMqttTopicPrefix());
        
        MqttClient *mqttClient = new MqttClient(properties, model);
        CecClient *cecClient = CecClient::getInstance(properties, model);

        mqttClient->connect();
        cecClient->connect();

        //TODO: set a trigger state to model, so changes before the init of all components are saved and then fired
        //update: only the last event is fired
        //TODO: clear command nodes at start
        model->retriggerInsertChangeRecursive();

        generalLogger.get()->info("Successfully initialized client.");
        while(!interrupt.load()){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        //FIXME:
        //update activeSourceDeviceId when turning tv on / off
        //update device 

        //TODO: TEST
        //toggle activeDevice
        //hdmi disconnect cec
        generalLogger.get()->info("Exiting program after user interrupt.");
        return 0;
    }catch(const PropertyException &propertyException){
        generalLogger.get()->error("Exception occured while reading property file: {}", propertyException.what());
    }catch(const std::exception &exception){
        generalLogger.get()->error("Exception occured: {}", exception.what());
    }
    generalLogger.get()->info("Exiting program after exception.");
    return -1;
}
