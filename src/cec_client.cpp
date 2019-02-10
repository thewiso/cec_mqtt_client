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

// cecloader.h uses std::cout _without_ including iosfwd or iostream
// Furthermore is uses cout and not std::cout
using std::cout;
using std::endl;

#include <libcec/cecloader.h>
#include "bcm_host.h"

CecClient *CecClient::singleton = nullptr;
//opcodes from cec_commands that should trigger a model update
const CEC::cec_opcode CecClient::RELEVANT_OPCODES[] = {
    CEC::cec_opcode::CEC_OPCODE_ACTIVE_SOURCE,      //is sent if TV is switched ON and
                                                    //is sent if external Device is switchted ON/OFF
    CEC::cec_opcode::CEC_OPCODE_STANDBY             //is sent if TV is switched OFF
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

CecClient::CecClient(const CecMqttClientProperties &properties, CecMqttClientModel *model){
    this->properties = properties;
    this->model = model;
    this->adapter = nullptr;
    this->callbacks = new CEC::ICECCallbacks();
    this->config = new CEC::libcec_configuration();
    this->logger = spdlog::get(Utilities::CEC_LOGGER_NAME);

    model->getGeneralModel()->getClientOSDNameCommand()->registerChangeHandler(std::bind( &CecClient::clientOSDNameCommandNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);

    bcm_host_init();

    callbacks->Clear();
    callbacks->sourceActivated = &static_sourceActivatedHandler;
    callbacks->commandReceived = &static_commandReceivedHandler;
    callbacks->alert = &static_alertHandler;

    config->Clear();    
    config->clientVersion = CEC::LIBCEC_VERSION_CURRENT;
    config->bActivateSource = 0;
    config->callbacks = callbacks;
    config->deviceTypes.Add(CEC::CEC_DEVICE_TYPE_PLAYBACK_DEVICE);

    copyOSDDeviceNameToConfig(properties.getCecDeviceName());
}

CecClient::~CecClient(){
    if(adapter != nullptr){
        adapter->Close();
        UnloadLibCec(adapter);
    }
}

CecClient *CecClient::getInstance(){
    //private method, so it can only be called from the callback handlers when the instance is already created
    return singleton;
}

CecClient *CecClient::getInstance(const CecMqttClientProperties &properties, CecMqttClientModel *model){
    //no need for thread safety, this should be called at program start in the good old main method
    if(singleton == nullptr){
        singleton = new CecClient(properties, model);
    }
    return singleton;
}

void CecClient::static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated){
    getInstance()->updateGeneralModel();
    //TODO: debug log
}

void CecClient::static_commandReceivedHandler(void* UNUSED, const CEC::cec_command* command){
    //if no poll command:
    if(command->opcode_set == 1){
        if (std::find(std::begin(RELEVANT_OPCODES), std::end(RELEVANT_OPCODES), command->opcode) != std::end(RELEVANT_OPCODES)){
            getInstance()->updateDeviceModel();
            getInstance()->updateGeneralModel();
        }
        //TODO: debug log
        if(command->opcode == CEC::cec_opcode::CEC_OPCODE_ACTIVE_SOURCE){
            CEC::cec_datapacket parameter = command->parameters;
            std::string parameterString;
            for(int i=0; i<parameter.size; i++){
                parameterString += ":" + std::to_string(parameter[i]);
            }
            // cout << "ActiveSource from " << CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(command->initiator) << " to " << CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(command->destination) << " parameter: '" << parameterString << "'\n";
        }
        // cout << "from " << CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(command->initiator) << " to " << CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(command->destination) << " command: " << std::hex << "0x" << (int)command->opcode << "\n";
    }
    
}

void CecClient::static_alertHandler(void* UNUSED, const CEC::libcec_alert alert, const CEC::libcec_parameter param){
    cout << "alert\n";
        //TODO: alert callbacks usage for connection lost handling!
        //TODO: debug log

}
        


void CecClient::connect(){
    logger.get()->info("Connecting to CEC Hub...");
    
    adapter = LibCecInitialise(config);
    if( !adapter )
    { 
        //TODO: throw exception?
        std::cerr << "Failed loading libcec.so\n"; 
    }
    std::array<CEC::cec_adapter_descriptor, 10> devices;
    int8_t devices_found = adapter->DetectAdapters(devices.data(), devices.size(), nullptr, false /*quickscan*/);
    if( devices_found <= 0)
    {
        std::cerr << "Could not automatically determine the cec adapter devices\n";
        UnloadLibCec(adapter);
        //TODO: throw exception
    }

    if(adapter->Open(devices[0].strComName)){
        //TODO: throw exception
    }

    logger.get()->info("Connecting to CEC Hub succedeed.");
    updateGeneralModel();
    updateDeviceModel();
}

void CecClient::updateGeneralModel(){
    logger.get()->debug("Updating GeneralModel");

    adapterMutex.lock();
    CEC::cec_logical_address activeSource = adapter->GetActiveSource();
    adapterMutex.unlock();

    model->getGeneralModel()->getActiveSourceLogicalAddress()->setValue(CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(activeSource));

    //TODO: updates after commands should be triggered (name changed -> configuration changed?)
}

void CecClient::updateDeviceModel(){
    logger.get()->debug("Updating DeviceModel");
    //TODO: log debug duration of method
    //TODO: when to use adapter->RescanActiveDevices ?
    adapterMutex.lock();
    CEC::cec_logical_addresses activeDevices = adapter->GetActiveDevices();
    adapterMutex.unlock();

    int activeDevicesLength = (sizeof(activeDevices.addresses)/sizeof(*activeDevices.addresses));
    std::vector<DeviceModel*> deviceModels = model->getDeviceModels();

    for(int i=0; i<activeDevicesLength; i++){
        CEC::cec_logical_address currentAddress = (CEC::cec_logical_address)i;
        std::string currentAddressString = CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(currentAddress);
        
        if(activeDevices[i]){
            
            adapterMutex.lock();
                std::string odsName = adapter->GetDeviceOSDName(currentAddress);
                CEC::cec_power_status powerStatus = adapter->GetDevicePowerStatus(currentAddress);
            adapterMutex.unlock();

            std::pair<DeviceModel*, bool> activeDevicePair = model->getOrCreateDeviceModel(currentAddressString);
            DeviceModel *activeDevice = activeDevicePair.first;
            activeDevice->getName()->setValue(odsName);
            activeDevice->getIsActive()->setValue(Utilities::TRUE_STRING_LITERAL);
            activeDevice->getPowerStatus()->setValue(CEC_POWER_STATUS_2_STRING_LITERAL.at(powerStatus));

            //if device was created by getOrCreateDeviceModel:
            if(activeDevicePair.second){
                activeDevice->getPowerStatusCommand()->registerChangeHandler(std::bind(&CecClient::devicePowerStateCommandNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);
            }
        }else{
            //set activity and power state of other, already created and now inactive devices
            auto iterator = std::find_if(deviceModels.begin(), deviceModels.end(), [currentAddressString] (DeviceModel *deviceModel) { return deviceModel->getLogicalAddress()->getValue() == currentAddressString; } ); 
            if(iterator != deviceModels.end()){
                (*iterator)->getIsActive()->setValue(Utilities::FALSE_STRING_LITERAL);
                (*iterator)->getPowerStatus()->setValue(CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::cec_power_status::CEC_POWER_STATUS_UNKNOWN));
            }

        }
    } 
}

void CecClient::clientOSDNameCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType){
    std::string deviceName = modelNode.getValue();
    if(!deviceName.empty()){

        copyOSDDeviceNameToConfig(deviceName);
        (*logger).debug("Changing client OSD name to {}...", deviceName);
        
        adapterMutex.lock();
        bool success = adapter->SetConfiguration(config);
        adapterMutex.unlock();

        if(success){
            (*logger).debug("Changing client OSD name to {} succedeed.", deviceName);
            updateDeviceModel();
        }else{
            (*logger).error("Failed to change client OSD name to {}.", deviceName);
        }

        modelNode.setValue(std::string());
    }
}

void CecClient::devicePowerStateCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeEventType modelNodeChangeEventType){
    std::string powerStateString = modelNode.getValue();
    if(!powerStateString.empty()){
        try{
            DeviceModel &deviceModel = dynamic_cast<DeviceModel&>(*(modelNode.getParent()->getParent()));
            
            std::string logicalAddressString = deviceModel.getLogicalAddress()->getValue();
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
                        (*logger).debug("Successfully turned on device {}", logicalAddressString);
                    }else{
                        (*logger).error("Could not turn on device {}", logicalAddressString);
                    }
                //standby device:
                }else if(powerStateString == CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::CEC_POWER_STATUS_STANDBY)){
                    adapterMutex.lock();
                    bool success = adapter->StandbyDevices(logicalAddress);
                    adapterMutex.unlock();

                    if(success){
                        (*logger).debug("Successfully turned device {} into standby", logicalAddressString);
                    }else{
                        (*logger).error("Could not standby device {}", logicalAddressString);
                    }
                }else{
                    (*logger).error("{} is not a valid power state to set", powerStateString);
                }
            }else{
                (*logger).error("Could not parse {} to a valid cec logical address", logicalAddressString);
            }
            modelNode.setValue(std::string());

        }catch(const std::bad_cast &ex){
            throw std::logic_error("Could not cast modelNode to DeviceModel in devicePowerStateCommandNodeChangeHandler");
        }
    }
}

void CecClient::copyOSDDeviceNameToConfig(std::string &deviceName){
    if(deviceName.size() > MAX_OSD_NAME_LENGTH){
        (*logger).warn("Given OSD device name '{}' has more than allowed {} characters and is shortened to this length", deviceName, MAX_OSD_NAME_LENGTH);
    }
    deviceName.resize(MAX_OSD_NAME_LENGTH);
    std::size_t length = MAX_OSD_NAME_LENGTH;
    deviceName.copy(config->strDeviceName, length);
}