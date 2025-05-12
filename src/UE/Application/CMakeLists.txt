project(UeApplication)
cmake_minimum_required(VERSION 3.12)

#aux_source_directory(. SRC_LIST)
#aux_source_directory(Ports SRC_LIST)
#aux_source_directory(States SRC_LIST)
#aux_source_directory(Models SRC_LIST)

#set(SRC_LIST ${SRC_LIST} Models/SmsDatabase.cpp)

set(APPLICATION_SOURCES
        Application.cpp
        Models/SmsDatabase.cpp
        Ports/BtsPort.cpp
        Ports/UserPort.cpp
        Ports/TimerPort.cpp
        States/BaseState.cpp
        States/ConnectingState.cpp
        States/ConnectedState.cpp
        States/NotConnectedState.cpp
        States/ViewingSmsListState.cpp
        States/ViewingSmsState.cpp
        States/ComposingSmsState.cpp
)

#add_library(${PROJECT_NAME} ${SRC_LIST})
add_library(${PROJECT_NAME} ${APPLICATION_SOURCES})

target_link_libraries(${PROJECT_NAME} Common)

target_include_directories(${PROJECT_NAME} PUBLIC .)
