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

//TODO: use lambda in signal function?
std::atomic<bool> interrupt(false); 
void handle_signal(int signal)
{
    interrupt.store(true);
}


int main(int argc, char* argv[])
{
    //TODO: pattern?
    //TODO: log file path, size, level as propertyfiles
    //1024 * 1024 * 5 = 5 Mebibyte, 2 log filesspdlog::level::debug
    auto sharedFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/cec_mqtt_client_log.txt", 1024 * 1024 * 5, 2);
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //TODO: property file: boolean logToConsole

    
    auto cecLogger = std::make_shared<spdlog::logger>("CEC", spdlog::sinks_init_list{sharedFileSink, consoleSink});
    auto mqttLogger = std::make_shared<spdlog::logger>("MQTT", spdlog::sinks_init_list{sharedFileSink, consoleSink});
    auto generalLogger = std::make_shared<spdlog::logger>(Utilities::GENERAL_LOGGER_NAME, spdlog::sinks_init_list{sharedFileSink, consoleSink});
    spdlog::register_logger(generalLogger);

    spdlog::level::level_enum logLevel = spdlog::level::level_enum::trace;
    generalLogger.get()->set_level(logLevel);
    mqttLogger.get()->set_level(logLevel);
    cecLogger.get()->set_level(logLevel);

    //TODO: implement usage of auto (e.g. in for loop iterator)
    //TODO: smart pointer??
    //TODO: general update command
    signal(SIGINT, handle_signal);

    try{
        generalLogger.get()->info("Reading property file...");
        CecMqttClientProperties properties;
        properties.readFile("cec_mqtt_client.conf");
        generalLogger.get()->info("Finished reading property file.");

        CecMqttClientModel *model = new CecMqttClientModel(properties.getMqttTopicPrefix());
        
        MqttClient *mqttClient = new MqttClient(properties, model, mqttLogger);
        CecClient *cecClient = CecClient::getInstance(properties, model, cecLogger);

        mqttClient->connect();
        cecClient->connect();

        while(!interrupt.load()){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        //FIXME:
        //update activeSourceDeviceId when turning tv on / off
        //update device 

        //TODO: TEST
        //toggle activeDevice
        //network disconnect mqtt
        //hdmi disconnect cec
        generalLogger.get()->info("Exiting program after user interrupt.");
        return 0;
    }catch(const PropertyException &propertyException){
        generalLogger.get()->error("Exception occured while reading property file: {}", propertyException.what());
    }catch(const std::exception &exception){
        generalLogger.get()->error("Unexpected exception occured: {}", exception.what());
    }
    generalLogger.get()->info("Exiting program after exception.");
    return -1;
}