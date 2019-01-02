#include "cec_client.h"

#include <array>
#include <string>
#include <iostream>
// cecloader.h uses std::cout _without_ including iosfwd or iostream
// Furthermore is uses cout and not std::cout
using std::cout;
using std::endl;

#include <libcec/cec.h>
#include <libcec/cecloader.h>
#include "bcm_host.h"


CecClient::CecClient(CecMqttClientProperties properties){
    this->properties = properties;
}

CecClient::~CecClient(){
    
}

void CecClient::scanDevices(){
    
    // Initialise the graphics pipeline for the raspberry pi. Yes, this is necessary.
    bcm_host_init();

    // Set up the CEC config and specify the keypress callback function
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

    // Get a cec adapter by initialising the cec library
    CEC::ICECAdapter* cec_adapter = LibCecInitialise(&cec_config);
    if( !cec_adapter )
    { 
        std::cerr << "Failed loading libcec.so\n"; 
        // return 1; 
    }

    // Try to automatically determine the CEC devices 
    std::array<CEC::cec_adapter_descriptor, 10> devices;
    int8_t devices_found = cec_adapter->DetectAdapters(devices.data(), devices.size(), nullptr, false /*quickscan*/);
    if( devices_found <= 0)
    {
        std::cerr << "Could not automatically determine the cec adapter devices\n";
        UnloadLibCec(cec_adapter);
        // return 1;
    }
    for(int i=0; i < devices_found; i++) {
        cout << devices[i].strComName << " " << devices[i].adapterType << "\n";
    }

    cout << "Open: " << std::to_string(cec_adapter->Open(devices[0].strComName) )  << "\n";

    cout << std::to_string(cec_adapter->PowerOnDevices())  << "\n";
    // cout << std::to_string(cec_adapter->StandbyDevices(CEC::CECDEVICE_TV))  << "\n";

   // cout << std::to_string(cec_adapter->AudioToggleMute()) << "\n";

   cout << "Adresses:\n";
   CEC::cec_logical_addresses logicalAdresses = cec_adapter->GetActiveDevices();
   int logicalAdressesLength = (sizeof(logicalAdresses.addresses)/sizeof(*logicalAdresses.addresses));
    for(int i=0; i<logicalAdressesLength;i++){
        if(logicalAdresses[i]){
            cout << cec_adapter->GetDeviceOSDName((CEC::cec_logical_address)i) << "\n";
        }
    } 
    
    
    // Close down and cleanup
    cec_adapter->Close();
    UnloadLibCec(cec_adapter);
}