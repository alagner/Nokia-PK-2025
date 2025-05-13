cmake_minimum_required(VERSION 3.12)
set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)
project(CPP_Workshop)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/internal_commands.cmake")

set_compiler_options()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(googletest)
unset(CMAKE_POLICY_VERSION_MINIMUM)
add_subdirectory(UE)
add_subdirectory(BTS)
add_subdirectory(COMMON)
