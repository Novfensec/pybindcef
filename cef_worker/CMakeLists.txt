cmake_minimum_required(VERSION 3.15)
project(cef_worker)

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

message(STATUS "Building worker with CEF_ROOT: ${CEF_ROOT}")

if(WIN32)
    add_executable(cef_worker platforms/win/cef_worker.cpp)
else()
    add_executable(cef_worker platforms/linux/cef_worker.cpp)
endif()

target_include_directories(cef_worker PRIVATE ${CEF_ROOT})

if(WIN32)
    target_link_libraries(cef_worker PRIVATE
        ${CEF_ROOT}\\Release\\libcef.lib
    )
else()
    target_link_libraries(cef_worker PRIVATE
        ${CEF_ROOT}/Release/libcef.so
    )
    set_target_properties(cef_worker PROPERTIES
        BUILD_RPATH "${CEF_ROOT}/Release"
        INSTALL_RPATH "${CEF_ROOT}/Release"
    )
endif()
