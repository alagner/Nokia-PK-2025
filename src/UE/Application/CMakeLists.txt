# UE/Application/CMakeLists.txt

add_library(UeApplication)

target_sources(UeApplication
    PRIVATE
        Application.cpp
        Context.cpp
        Ports/BtsPort.cpp
        Ports/UserPort.cpp
        Ports/TimerPort.cpp
        States/BaseState.cpp
        States/NotConnectedState.cpp
        States/ConnectingState.cpp
        States/ConnectedState.cpp
        States/ViewingSmsListState.cpp
        States/ViewingSingleSmsState.cpp
        States/ComposingSmsState.cpp
)

target_include_directories(UeApplication
    PUBLIC # Export Application include directory
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE # Internal includes
         ${CMAKE_CURRENT_SOURCE_DIR}/Data
)

target_link_libraries(UeApplication
    PUBLIC # Application interface depends on these
        UeApplicationEnvironment
        Common
)

# --- Unit Tests ---
if (BUILD_TESTING)
    add_subdirectory(Tests)
endif()