cmake_minimum_required(VERSION 3.15)
project(cef_worker)

set(CMAKE_CXX_STANDARD 20)

if(DEFINED ENV{CEF_ROOT})
    set(CEF_ROOT $ENV{CEF_ROOT})
else()
    set(CEF_ROOT "~/Downloads/cef_binary")
endif()

message(STATUS "Building worker with CEF_ROOT: ${CEF_ROOT}")

add_executable(cef_worker cef_worker.cpp)

target_include_directories(cef_worker PRIVATE ${CEF_ROOT})

target_link_libraries(cef_worker PRIVATE 
    ${CEF_ROOT}/Release/libcef.so
)

set_target_properties(cef_worker PROPERTIES
    BUILD_RPATH "${CEF_ROOT}/Release"
    INSTALL_RPATH "${CEF_ROOT}/Release"
)


