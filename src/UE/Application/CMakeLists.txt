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
        # CORRECTED: Add new state source files
        States/ViewingSmsListState.cpp
        States/ViewingSingleSmsState.cpp
        # Add other State .cpp files here later using relative paths
)

target_include_directories(UeApplication
    PUBLIC # Export Application include directory
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE # Internal includes
         ${CMAKE_CURRENT_SOURCE_DIR}/Data # Needed for SmsData.hpp
         # Other PRIVATE includes might not be strictly necessary if using relative paths
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