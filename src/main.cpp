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

const std::string LOG_FILE_PATH = "cec_mqtt_client_log.txt";
//1024 * 1024 * 5 = 5 Mebibyte
const int LOG_FILE_SIZE = 1024 * 1024 * 5;

void handle_signal(int signal)
{
    interrupt.store(true);
}


int main(int argc, char* argv[])
{
    auto sharedFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LOG_FILE_PATH, LOG_FILE_SIZE, 2);
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    spdlog::sinks_init_list loggerSinks = {sharedFileSink, consoleSink};
    
    auto cecLogger = std::make_shared<spdlog::logger>(Utilities::CEC_LOGGER_NAME, loggerSinks);
    auto mqttLogger = std::make_shared<spdlog::logger>(Utilities::MQTT_LOGGER_NAME, loggerSinks);
    auto generalLogger = std::make_shared<spdlog::logger>(Utilities::GENERAL_LOGGER_NAME, loggerSinks);
    spdlog::register_logger(cecLogger);
    spdlog::register_logger(mqttLogger);
    spdlog::register_logger(generalLogger);

    signal(SIGINT, handle_signal);

    try{
        CecMqttClientProperties properties;
        properties.readFile("cec_mqtt_client.conf");

        if(!properties.getLoggerLogToConsole()){
            consoleSink->set_level(spdlog::level::level_enum::off);
        }
        SpdLogLevel logLevel = properties.getLoggerLevel();
        generalLogger->set_level(logLevel);
        mqttLogger->set_level(logLevel);
        cecLogger->set_level(logLevel);

        generalLogger->info("Finished reading property file.");
        generalLogger->info("Initializing client...");

        auto model = std::make_shared<CecMqttClientModel>(properties.getMqttTopicPrefix());
        model->init();

        MqttClient mqttClient(properties, model);
        CecClient &cecClient = CecClient::getInstance(properties, model);

        mqttClient.connect();
        cecClient.connect();

        generalLogger->info("Successfully initialized client.");

        model->resumeChangeEvents();

        while(!interrupt.load()){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        generalLogger->info("Exiting program after user interrupt.");
        return 0;
    }catch(const PropertyException &propertyException){
        generalLogger->error("Exception occured while reading property file: {}", propertyException.what());
    }catch(const std::exception &exception){
        generalLogger->error("Exception occured: {}", exception.what());
    }
    generalLogger->info("Exiting program after exception.");
    return -1;
}
