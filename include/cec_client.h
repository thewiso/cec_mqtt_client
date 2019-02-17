#pragma once

#include "cec_mqtt_client_properties.h"
#include "cec_mqtt_client_model.h"

#include "spdlog/logger.h"
#include <libcec/cec.h>

#include <mutex>
#include <memory>

//The class is using a global singleton to make it available for function callbacks from libCEC
class CecClient
{
    public:
        ~CecClient();
        void connect();

        static CecClient &getInstance(CecMqttClientProperties &properties, std::shared_ptr<CecMqttClientModel> model);

    private:
        CecClient(const CecMqttClientProperties &properties, std::shared_ptr<CecMqttClientModel> model);
        void updateGeneralModel();
        void updateDeviceModel();
        void clientOSDNameCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType);
        void devicePowerStateCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType);
        void copyOSDDeviceNameToConfig(std::string &deviceName);

        CecMqttClientProperties properties;
        std::shared_ptr<CecMqttClientModel> model;
        CEC::ICECAdapter *adapter;
        std::shared_ptr<CEC::ICECCallbacks> callbacks;
        std::shared_ptr<CEC::libcec_configuration> config;
        std::mutex adapterMutex;
        std::shared_ptr<spdlog::logger> logger;
        
        static void static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated);
        static void static_commandReceivedHandler(void* UNUSED, const CEC::cec_command* command);
        static void static_alertHandler(void* UNUSED, const CEC::libcec_alert alert, const CEC::libcec_parameter param);

        static CecClient &getInstance();
        static CecClient *singleton;

        static const std::map<CEC::cec_logical_address, std::string> CEC_LOGICAL_ADRESS_2_STRING_LITERAL;
        static const std::map<CEC::cec_power_status, std::string> CEC_POWER_STATUS_2_STRING_LITERAL;
        static const std::map<CEC::libcec_alert, std::string> CEC_ALERT_2_STRING_LITERAL;
        static const CEC::cec_opcode RELEVANT_OPCODES[];
        static const int MAX_OSD_NAME_LENGTH;

};