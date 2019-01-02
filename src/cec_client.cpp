#include "cec_client.h"

#include <array>
#include <string>
#include <iostream>
// cecloader.h uses std::cout _without_ including iosfwd or iostream
// Furthermore is uses cout and not std::cout
using std::cout;
using std::endl;

#include <libcec/cecloader.h>
#include "bcm_host.h"


CecClient::CecClient(CecMqttClientProperties properties, CecMqttClientModel &model){
    this->properties = properties;
    this->model = &model;
    this->cecAdapter = nullptr;
}

CecClient::~CecClient(){
    if(cecAdapter != nullptr){
        cecAdapter->Close();
        UnloadLibCec(cecAdapter);
    }
}

void CecClient::init(){
    bcm_host_init();

    // CEC::ICECCallbacks        cec_callbacks;
    CEC::libcec_configuration cec_config;
    cec_config.Clear();
    // cec_callbacks.Clear();

    const std::string devicename("CECExample");
    devicename.copy(cec_config.strDeviceName, std::min(devicename.size(),13u) );
    
    cec_config.clientVersion       = CEC::LIBCEC_VERSION_CURRENT;
    cec_config.bActivateSource     = 0;
    // cec_config.callbacks           = &cec_callbacks;
    cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_PLAYBACK_DEVICE);

    // cec_callbacks.keyPress    = &on_keypress;

    cecAdapter = LibCecInitialise(&cec_config);
    if( !cecAdapter )
    { 
        //TODO: throw exception?
        std::cerr << "Failed loading libcec.so\n"; 
    }
    std::array<CEC::cec_adapter_descriptor, 10> devices;
    int8_t devices_found = cecAdapter->DetectAdapters(devices.data(), devices.size(), nullptr, false /*quickscan*/);
    if( devices_found <= 0)
    {
        std::cerr << "Could not automatically determine the cec adapter devices\n";
        UnloadLibCec(cecAdapter);
        //TODO: throw exception
    }

    if(cecAdapter->Open(devices[0].strComName)){
        //TODO: throw exception
    }
    updateModel();
}

void CecClient::updateModel(){

    // cout << std::to_string(cecAdapter->PowerOnDevices())  << "\n";

//     cout << "Adresses:\n";
    // CEC::cec_logical_addresses logicalAdresses = cecAdapter->GetActiveDevices();
//    int logicalAdressesLength = (sizeof(logicalAdresses.addresses)/sizeof(*logicalAdresses.addresses));
//     for(int i=0; i<logicalAdressesLength;i++){
//         if(logicalAdresses[i]){
//             cout << cecAdapter->GetDeviceOSDName((CEC::cec_logical_address)i) << "\n";
//         }
//     } 
    model->getGeneralModel()->getActiveSourceDeviceId()->setValue(std::to_string((int)cecAdapter->GetActiveSource()));


    

}