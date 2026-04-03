cmake_minimum_required(VERSION 3.15)
project(pybindcef)

set(CMAKE_CXX_STANDARD 20)

if(DEFINED ENV{CEF_ROOT})
    set(CEF_ROOT $ENV{CEF_ROOT})
else()
    set(CEF_ROOT "$ENV{HOME}/Downloads/cef_binary")
endif()

if(DEFINED ENV{CEF_DLL_WRAPPER_ROOT})
    set(CEF_DLL_WRAPPER_ROOT $ENV{CEF_DLL_WRAPPER_ROOT})
else()
    set(CEF_DLL_WRAPPER_ROOT "${CEF_ROOT}/build/libcef_dll_wrapper")
endif()


message(STATUS "CEF_ROOT: ${CEF_ROOT}")
message(STATUS "CEF_DLL_WRAPPER_ROOT: ${CEF_DLL_WRAPPER_ROOT}")

find_package(Python COMPONENTS Interpreter Development REQUIRED)

set(PYBIND11_FINDPYTHON ON)

find_package(pybind11 REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(${CEF_ROOT} src/)

pybind11_add_module(pybindcef src/bridge.cpp)

target_link_libraries(pybindcef PRIVATE
    ${CEF_DLL_WRAPPER_ROOT}/libcef_dll_wrapper.a
    ${CEF_ROOT}/Release/libcef.so
)

set_target_properties(pybindcef PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

