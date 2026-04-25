cmake_minimum_required(VERSION 3.15)
project(cmake-gadget)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# SDL3 Paths - Customize these for your environment
set(SDL3_INCLUDE "E:/sdlApp/SDL3-devel-3.4.4-VC/SDL3-3.4.4/include")
set(SDL3_LIB "E:/sdlApp/SDL3-devel-3.4.4-VC/SDL3-3.4.4/lib/x64")

include_directories(${SDL3_INCLUDE})
link_directories(${SDL3_LIB})

# Source and Executable
add_executable(cmake-gadget WIN32 "launcher.c" "resource.rc")

# Release Mode Optimizations
if(MSVC)
    target_compile_options(cmake-gadget PRIVATE $<$<CONFIG:Release>:/Ox /GL /Gw>)
    target_link_options(cmake-gadget PRIVATE $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>)
endif()

target_link_libraries(cmake-gadget SDL3)