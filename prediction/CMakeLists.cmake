cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
add_definitions(-std=gnu++0x)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

include_directories(../third_party/android_prediction)

add_executable(haha 
	dictionary_service.cc
    dictionary_service.h
    input_info.cc
    input_info.h
    key_set.h
    prediction_service_impl.cc
    prediction_service_impl.h
    proximity_info_factory.cc
    proximity_info_factory.h
    touch_position_correction.cc
    touch_position_correction.h
)