cmake_minimum_required(VERSION 3.0.0)
project(cec_mqtt_client VERSION 0.1.0)

include(CTest)
enable_testing()

set(SRCDIR ${PROJECT_SOURCE_DIR}/src)
set(SRCFILES 
    ${SRCDIR}/main.cpp 
    ${SRCDIR}/cec-simplest.cpp
    ${SRCDIR}/properties.cpp
    ${SRCDIR}/cec_mqtt_client_properties.cpp
)

#executatble definition
add_executable(cec_mqtt_client ${SRCFILES})

#include files
target_include_directories(cec_mqtt_client PUBLIC include)


#libraries
find_library(PAHO_MQTT_C libpaho-mqtt3a.so)
find_library(PAHO_MQTT_CPP libpaho-mqttpp3.so)
find_library(LIB_CEC libcec.so)

message(STATUS "Paho MQTT C found at    ${PAHO_MQTT_C}")
message(STATUS "Paho MQTT C++ found at  ${PAHO_MQTT_CPP}")
message(STATUS "LibCEC found at         ${LIB_CEC}")

target_link_libraries(cec_mqtt_client ${LIB_CEC} ${PAHO_MQTT_C} ${PAHO_MQTT_CPP})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
