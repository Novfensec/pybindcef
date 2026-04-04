cmake_minimum_required(VERSION 3.15)
project(pybindcef)

set(CMAKE_CXX_STANDARD 20)

if(DEFINED ENV{CEF_ROOT})
    set(CEF_ROOT $ENV{CEF_ROOT})
else()
    if(WIN32)
        set(CEF_ROOT "$ENV{USERPROFILE}\\Downloads\\cef_binary")
    else()
        set(CEF_ROOT "$ENV{HOME}/Downloads/cef_binary")
    endif()
endif()

if(DEFINED ENV{CEF_DLL_WRAPPER_ROOT})
    set(CEF_DLL_WRAPPER_ROOT $ENV{CEF_DLL_WRAPPER_ROOT})
else()
    if(WIN32)
        set(CEF_DLL_WRAPPER_ROOT "${CEF_ROOT}\\build\\libcef_dll_wrapper\\Release")
    else()
        set(CEF_DLL_WRAPPER_ROOT "${CEF_ROOT}/build/libcef_dll_wrapper")
    endif()
endif()

message(STATUS "CEF_ROOT: ${CEF_ROOT}")
message(STATUS "CEF_DLL_WRAPPER_ROOT: ${CEF_DLL_WRAPPER_ROOT}")

find_package(Python COMPONENTS Interpreter Development REQUIRED)
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)

include_directories(${CEF_ROOT} src/)

set(SOURCES src/bridge.cpp)

if(WIN32)
    message(STATUS "Configuring pybindcef for Windows")
    add_compile_definitions(NOMINMAX)
    list(APPEND SOURCES 
        src/platforms/win/app_win.cpp
        src/platforms/win/gpu_mapper.cpp
    )
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    message(STATUS "Configuring pybindcef for Linux")
    list(APPEND SOURCES 
        src/platforms/linux/app_linux.cpp
    )
endif()

pybind11_add_module(pybindcef ${SOURCES})

if(WIN32)
    target_link_libraries(pybindcef PRIVATE
        ${CEF_DLL_WRAPPER_ROOT}/libcef_dll_wrapper.lib
        ${CEF_ROOT}/Release/libcef.lib
        d3d11.lib
        opengl32.lib
    )
    add_custom_command(TARGET pybindcef POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CEF_ROOT}/Release/libcef.dll $<TARGET_FILE_DIR:pybindcef>
    )
    set_property(TARGET pybindcef PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
else()
    target_link_libraries(pybindcef PRIVATE
        ${CEF_DLL_WRAPPER_ROOT}/libcef_dll_wrapper.a
        ${CEF_ROOT}/Release/libcef.so
    )
    set_target_properties(pybindcef PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endif()

install(TARGETS pybindcef DESTINATION .)
