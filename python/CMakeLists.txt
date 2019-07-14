find_package(pybind11 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/prediction) 
pybind11_add_module(suggestr MODULE bindings.cpp)
target_link_libraries(suggestr PRIVATE prediction_lib)

configure_file(test.py test.py COPYONLY)