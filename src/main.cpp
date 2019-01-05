#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "cec_mqtt_client_properties.h"
#include "mqtt_client.h"
#include "cec_client.h"
#include "cec_mqtt_client_model.h"

//TODO: use lambda in signal function?
std::atomic<bool> interrupt(false); 
void handle_signal(int signal)
{
    // interrupt.store(true);
}


int main(int argc, char* argv[])
{
    //TODO: implement usage of auto (e.g. in for loop iterator)
    //TODO: smart pointer??
    //TODO: general update command
    signal(SIGINT, handle_signal);


    //TODO: log file path, size, level as propertyfiles
    //1024 * 1024 * 5 = 5 Mebibyte, 2 log filesspdlog::level::debug
    auto sharedFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/cec_mqtt_client_log.txt", 1024 * 1024 * 5, 2);
    auto cecLogger = std::make_shared<spdlog::logger>("CEC", sharedFileSink);
    auto mqttLogger = std::make_shared<spdlog::logger>("MQTT", sharedFileSink);
    auto generalLogger = std::make_shared<spdlog::logger>("GENERAL", sharedFileSink);

    spdlog::level::level_enum logLevel = spdlog::level::level_enum::debug;
    cecLogger.get()->set_level(logLevel);
    mqttLogger.get()->set_level(logLevel);
    generalLogger.get()->set_level(logLevel);

    CecMqttClientProperties properties;
    properties.readFile("cec_mqtt_client.conf");

    CecMqttClientModel *model = new CecMqttClientModel(properties.getMqttTopicPrefix());
    
    MqttClient *mqttClient = new MqttClient(properties, model, mqttLogger);
    CecClient *cecClient = CecClient::getInstance(properties, model, cecLogger);

    mqttClient->init();
    cecClient->init();

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
    return 0;
}