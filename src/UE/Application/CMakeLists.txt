project(UeApplication)
cmake_minimum_required(VERSION 3.12)

set(APPLICATION_SOURCES
        Application.cpp
        SmsStorage.cpp

        Ports/BtsPort.cpp
        Ports/UserPort.cpp
        Ports/TimerPort.cpp

        States/BaseState.cpp
        States/CallState.cpp
        States/ConnectingState.cpp
        States/ConnectedState.cpp
        States/NotConnectedState.cpp
        States/ViewListSmsState.cpp
        States/ViewSmsState.cpp
        States/ComposeSmsState.cpp
)

add_library(${PROJECT_NAME} ${APPLICATION_SOURCES})


target_link_libraries(${PROJECT_NAME} Common)

target_include_directories(${PROJECT_NAME} PUBLIC .)
