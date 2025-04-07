project(UeApplication)
cmake_minimum_required(VERSION 3.12)

# --- Explicitly list ALL source (.cpp) files for this library ---
set(APPLICATION_SOURCES
    # Files in the Application/ directory
    Application.cpp
    SmsDb.cpp

    # Files in the Application/Ports/ directory
    Ports/BtsPort.cpp
    Ports/UserPort.cpp
    Ports/TimerPort.cpp 

    # Files in the Application/States/ directory
    States/BaseState.cpp
    States/ConnectingState.cpp
    States/ConnectedState.cpp          
    States/NotConnectedState.cpp
    States/ViewingSmsListState.cpp     
    States/ViewingSingleSmsState.cpp   
    States/ComposingSmsState.cpp
    States/IncomingCallState.cpp
    States/TalkingState.cpp
    States/CallingState.cpp
)


# Define the library target using the explicit list
add_library(${PROJECT_NAME} ${APPLICATION_SOURCES})


# Make sure "Common" is a valid CMake target defined elsewhere
target_link_libraries(${PROJECT_NAME} Common)

# Add include directories to help find headers

target_include_directories(${PROJECT_NAME} PUBLIC .) 