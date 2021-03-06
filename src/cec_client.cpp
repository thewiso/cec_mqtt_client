#include "cec_client.h"
#include "utilities.h"
#include "spdlog/spdlog.h"
#include "device_model.h"

#include <algorithm> 
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <iomanip>

// cecloader.h uses std::cout _without_ including iosfwd or iostream
// Furthermore is uses cout and not std::cout
using std::cout;
using std::endl;

#include <libcec/cecloader.h>
#include "bcm_host.h"

//opcodes from cec_commands that should trigger a model update
const CEC::cec_opcode CecClient::RELEVANT_OPCODES[] = {
    CEC::cec_opcode::CEC_OPCODE_REQUEST_ACTIVE_SOURCE,
    CEC::cec_opcode::CEC_OPCODE_ACTIVE_SOURCE,              //is sent if TV is switched ON and
                                                            //is sent if external Device is switchted ON/OFF
    CEC::cec_opcode::CEC_OPCODE_REPORT_POWER_STATUS,        //is sent if TV is switched OFF
    CEC::cec_opcode::CEC_OPCODE_STANDBY
    
};

const int CecClient::MAX_OSD_NAME_LENGTH = 13;

const std::map<CEC::cec_logical_address, std::string> CecClient::CEC_LOGICAL_ADRESS_2_STRING_LITERAL = {
    {CEC::cec_logical_address::CECDEVICE_UNKNOWN         , "UNKNOWN"},
    {CEC::cec_logical_address::CECDEVICE_TV              , "TV"},
    {CEC::cec_logical_address::CECDEVICE_RECORDINGDEVICE1, "RECORDINGDEVICE1"},
    {CEC::cec_logical_address::CECDEVICE_RECORDINGDEVICE2, "RECORDINGDEVICE2"},
    {CEC::cec_logical_address::CECDEVICE_TUNER1          , "TUNER1"},
    {CEC::cec_logical_address::CECDEVICE_PLAYBACKDEVICE1 , "PLAYBACKDEVICE1"},
    {CEC::cec_logical_address::CECDEVICE_AUDIOSYSTEM     , "AUDIOSYSTEM"},
    {CEC::cec_logical_address::CECDEVICE_TUNER2          , "TUNER2"},
    {CEC::cec_logical_address::CECDEVICE_TUNER3          , "TUNER3"},
    {CEC::cec_logical_address::CECDEVICE_PLAYBACKDEVICE2 , "PLAYBACKDEVICE2"},
    {CEC::cec_logical_address::CECDEVICE_RECORDINGDEVICE3, "RECORDINGDEVICE3"},
    {CEC::cec_logical_address::CECDEVICE_TUNER4          , "TUNER4"},
    {CEC::cec_logical_address::CECDEVICE_PLAYBACKDEVICE3 , "PLAYBACKDEVICE3"},
    {CEC::cec_logical_address::CECDEVICE_RESERVED1       , "RESERVED1"},
    {CEC::cec_logical_address::CECDEVICE_RESERVED2       , "RESERVED2"},
    {CEC::cec_logical_address::CECDEVICE_FREEUSE         , "FREEUSE"},
    {CEC::cec_logical_address::CECDEVICE_UNREGISTERED    , "UNREGISTERED"},
    {CEC::cec_logical_address::CECDEVICE_BROADCAST       , "BROADCAST"}
};

const std::map<CEC::cec_power_status, std::string> CecClient::CEC_POWER_STATUS_2_STRING_LITERAL = {
    {CEC::cec_power_status::CEC_POWER_STATUS_ON                         , "ON"},
    {CEC::cec_power_status::CEC_POWER_STATUS_STANDBY                    , "STANDBY"},
    {CEC::cec_power_status::CEC_POWER_STATUS_IN_TRANSITION_STANDBY_TO_ON, "IN_TRANSITION_STANDBY_TO_ON"},
    {CEC::cec_power_status::CEC_POWER_STATUS_IN_TRANSITION_ON_TO_STANDBY, "IN_TRANSITION_ON_TO_STANDBY"},
    {CEC::cec_power_status::CEC_POWER_STATUS_UNKNOWN                    , "UNKNOWN"}
};

const std::map<CEC::libcec_alert, std::string> CecClient::CEC_ALERT_2_STRING_LITERAL = {
    {CEC::libcec_alert::CEC_ALERT_SERVICE_DEVICE        , "Service device"},
    {CEC::libcec_alert::CEC_ALERT_CONNECTION_LOST       , "Connection lost"},
    {CEC::libcec_alert::CEC_ALERT_PERMISSION_ERROR      , "Permission error"},
    {CEC::libcec_alert::CEC_ALERT_PORT_BUSY             , "Port busy"},
    {CEC::libcec_alert::CEC_ALERT_PHYSICAL_ADDRESS_ERROR, "Physical address error"},
    {CEC::libcec_alert::CEC_ALERT_TV_POLL_FAILED        , "TV poll failed"}
};

CecClient *CecClient::singleton = nullptr;

CEC::cec_command CecClient::lastCommand;
std::mutex CecClient::lastCommandMutex;


CecClient::CecClient(const CecMqttClientProperties &properties, std::shared_ptr<CecMqttClientModel> model){
    this->properties = properties;
    this->model = model;
    this->adapter = nullptr;
    this->logger = spdlog::get(Utilities::CEC_LOGGER_NAME);

    model->getGeneralModel().getClientOSDNameCommand().registerChangeHandler(std::bind( &CecClient::clientOSDNameCommandNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);

    bcm_host_init();

    callbacks = std::make_shared<CEC::ICECCallbacks>();
    callbacks->Clear();
    callbacks->sourceActivated = &static_sourceActivatedHandler;
    callbacks->commandReceived = &static_commandReceivedHandler;
    callbacks->alert = &static_alertHandler;

    config = std::make_shared<CEC::libcec_configuration>();
    config->Clear();    
    config->clientVersion = CEC::LIBCEC_VERSION_CURRENT;
    config->bActivateSource = 0;
    config->callbacks = callbacks.get();
    config->deviceTypes.Add(CEC::CEC_DEVICE_TYPE_PLAYBACK_DEVICE);

    std::string deviceName = this->properties.getCecDeviceName();
    copyOSDDeviceNameToConfig(deviceName);
}

CecClient::~CecClient(){
    if(adapter != nullptr){
        adapter->Close();
        UnloadLibCec(adapter);
        delete adapter;
    }
}

CecClient &CecClient::getInstance(){
    //private method, so it can only be called from the callback handlers when the instance is already created
    return *singleton;
}

CecClient &CecClient::getInstance(CecMqttClientProperties &properties, std::shared_ptr<CecMqttClientModel> model){
    //no need for thread safety, this should be called at program start in the good old main method
    if(singleton == nullptr){
        singleton = new CecClient(properties, model);
    }
    return *singleton;
}

void CecClient::static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated){
    getInstance().logger->debug("CEC adapter reported a new source actived: {}", CecClient::CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(logicalAddress));
    getInstance().updateGeneralModel();
}

void CecClient::static_commandReceivedHandler(void* UNUSED, const CEC::cec_command* command){
    //if no poll command:
    if(command->opcode_set == 1){
        if(getInstance().logger->should_log(spdlog::level::level_enum::trace)){
            std::string initiator = CecClient::CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(command->initiator);
            std::string destination = CecClient::CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(command->destination);
            std::stringstream commandStream;
            commandStream << std::hex << (int)command->initiator << (int)command->destination << ":";
            commandStream << std::setfill('0') << std::setw (2) << (int)command->opcode;
            for(int i = 0; i< command->parameters.size; i++){  
                commandStream << ":" << std::setfill('0') << std::setw (2) << (int)command->parameters.data[i];
            }
            getInstance().logger->trace("CEC adapter reported command recieved from {} to {}: {} (For more information use www.cec-o-matic.com", initiator, destination, commandStream.str());
        }
        
        //command->initiator == CEC::cec_logical_address::CECDEVICE_TV &&
        if (std::find(std::begin(RELEVANT_OPCODES), std::end(RELEVANT_OPCODES), command->opcode) != std::end(RELEVANT_OPCODES)){
            lastCommandMutex.lock();
                if(lastCommand.initiator != command->initiator
                || lastCommand.destination != command->destination
                || lastCommand.opcode != command->opcode){

                    lastCommand = *command;
                    lastCommandMutex.unlock();

                    getInstance().updateDeviceModel();
                    getInstance().updateGeneralModel();
                }

            lastCommandMutex.unlock();
          
        }
    }
    
}

void CecClient::static_alertHandler(void* UNUSED, const CEC::libcec_alert alert, const CEC::libcec_parameter param){
    cout << "alert\n";
    getInstance().logger->warn("CEC adapter reported an alert: {}", CecClient::CEC_ALERT_2_STRING_LITERAL.at(alert));
}
        


void CecClient::connect(){
    logger->info("Connecting to CEC Hub...");
    
    adapter = LibCecInitialise(config.get());
    if(!adapter)
    { 
        logger->error("Failed loading libcec.so"); 
        throw std::runtime_error("Could not connect to CEC Hub");
    }
    std::array<CEC::cec_adapter_descriptor, 10> devices;
    int8_t devices_found = adapter->DetectAdapters(devices.data(), devices.size(), nullptr, false /*quickscan*/);
    if(devices_found <= 0)
    {
        logger->error("Could not automatically determine the cec adapter devices");
        UnloadLibCec(adapter);
        throw std::runtime_error("Could not connect to CEC Hub");
    }

    if(!adapter->Open(devices[0].strComName)){
        logger->error("Failed to open the CEC device on port {}", devices[0].strComName);
        UnloadLibCec(adapter);
        throw std::runtime_error("Could not connect to CEC Hub");
    }

    logger->info("Connecting to CEC Hub succedeed.");
    updateGeneralModel();
    updateDeviceModel();
}

void CecClient::updateGeneralModel(){
    logger->debug("Updating GeneralModel");

    adapterMutex.lock();
    CEC::cec_logical_address activeSource = adapter->GetActiveSource();
    adapterMutex.unlock();

    model->getGeneralModel().getActiveSourceLogicalAddress().setValue(CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(activeSource));
}

void CecClient::updateDeviceModel(){
    logger->debug("Updating DeviceModel");
    auto start = std::chrono::high_resolution_clock::now();

    adapterMutex.lock();
        adapter->RescanActiveDevices();
        CEC::cec_logical_addresses activeDevices = adapter->GetActiveDevices();
    adapterMutex.unlock();

    int activeDevicesLength = (sizeof(activeDevices.addresses)/sizeof(*activeDevices.addresses));
    std::vector<std::shared_ptr<DeviceModel>> deviceModels = model->getDeviceModels();
    
    for(int i=0; i<activeDevicesLength; i++){
        CEC::cec_logical_address currentAddress = (CEC::cec_logical_address)i;
        std::string currentAddressString = CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(currentAddress);

        if(activeDevices[i]){
            adapterMutex.lock();
                std::string odsName = adapter->GetDeviceOSDName(currentAddress);
                CEC::cec_power_status powerStatus = adapter->GetDevicePowerStatus(currentAddress);
            adapterMutex.unlock();

            std::pair<DeviceModel&, bool> activeDevicePair = model->getOrCreateDeviceModel(currentAddressString);
            DeviceModel &activeDevice = activeDevicePair.first;
            activeDevice.getName().setValue(odsName);
            activeDevice.getIsActive().setValue(Utilities::TRUE_STRING_LITERAL);
            activeDevice.getPowerStatus().setValue(CEC_POWER_STATUS_2_STRING_LITERAL.at(powerStatus));

            //if device was created by getOrCreateDeviceModel:
            if(activeDevicePair.second){
                activeDevice.getPowerStatusCommand().registerChangeHandler(std::bind(&CecClient::devicePowerStateCommandNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);
            }
        }else{
            //set activity and power state of other, already created and now inactive devices
            auto iterator = std::find_if(deviceModels.begin(), deviceModels.end(), [currentAddressString] (std::shared_ptr<DeviceModel> &deviceModel) { return (*deviceModel).getLogicalAddress().getValue() == currentAddressString; } ); 
            if(iterator != deviceModels.end()){
                (*iterator)->getIsActive().setValue(Utilities::FALSE_STRING_LITERAL);
                (*iterator)->getPowerStatus().setValue(CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::cec_power_status::CEC_POWER_STATUS_UNKNOWN));
            }

        }
    } 

    if(logger->should_log(spdlog::level::level_enum::debug)){
        auto finish = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
        logger->debug("Updating DeviceModel took {} milliseconds", elapsed.count());
    }
}

void CecClient::clientOSDNameCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType){
    std::string deviceName = modelNode.getValue();
    if(!deviceName.empty()){

        copyOSDDeviceNameToConfig(deviceName);
        logger->debug("Changing client OSD name to {}...", deviceName);
        
        adapterMutex.lock();
        bool success = adapter->SetConfiguration(config.get());
        adapterMutex.unlock();

        if(success){
            logger->debug("Changing client OSD name to {} succedeed.", deviceName);
            updateDeviceModel();
        }else{
            logger->error("Failed to change client OSD name to {}.", deviceName);
        }

        modelNode.setValue(std::string());
    }
}

void CecClient::devicePowerStateCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType){
    std::string powerStateString = modelNode.getValue();
    if(!powerStateString.empty()){
        try{
            DeviceModel &deviceModel = dynamic_cast<DeviceModel&>(modelNode.getParent().getParent());
            
            std::string logicalAddressString = deviceModel.getLogicalAddress().getValue();
            auto foundLogicalAddressIterator = std::find_if(CEC_LOGICAL_ADRESS_2_STRING_LITERAL.begin(), CEC_LOGICAL_ADRESS_2_STRING_LITERAL.end(), [logicalAddressString] (auto pair) { return pair.second == logicalAddressString; } );
            
            if(foundLogicalAddressIterator != CEC_LOGICAL_ADRESS_2_STRING_LITERAL.end()){
                CEC::cec_logical_address logicalAddress = foundLogicalAddressIterator->first;
                Utilities::ToUpper(powerStateString);

                //power on device:
                if(powerStateString == CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::CEC_POWER_STATUS_ON)){
                    adapterMutex.lock();
                    bool success = adapter->PowerOnDevices(logicalAddress);
                    adapterMutex.unlock();

                    if(success){
                        logger->debug("Successfully turned on device {}", logicalAddressString);
                    }else{
                        logger->error("Could not turn on device {}", logicalAddressString);
                    }
                //standby device:
                }else if(powerStateString == CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::CEC_POWER_STATUS_STANDBY)){
                    adapterMutex.lock();
                    bool success = adapter->StandbyDevices(logicalAddress);
                    adapterMutex.unlock();

                    if(success){
                        logger->debug("Successfully turned device {} into standby", logicalAddressString);
                    }else{
                        logger->error("Could not standby device {}", logicalAddressString);
                    }
                }else{
                    logger->error("{} is not a valid power state to set", powerStateString);
                }
            }else{
                logger->error("Could not parse {} to a valid cec logical address", logicalAddressString);
            }
            modelNode.setValue(std::string());

        }catch(const std::bad_cast &ex){
            throw std::logic_error("Could not cast modelNode to DeviceModel in devicePowerStateCommandNodeChangeHandler");
        }
    }
}

void CecClient::copyOSDDeviceNameToConfig(std::string &deviceName){
    if(deviceName.size() > MAX_OSD_NAME_LENGTH){
        logger->warn("Given OSD device name '{}' has more than allowed {} characters and is shortened to this length", deviceName, MAX_OSD_NAME_LENGTH);
    }
    deviceName.resize(MAX_OSD_NAME_LENGTH);
    std::size_t length = MAX_OSD_NAME_LENGTH;
    deviceName.copy(config->strDeviceName, length);
}