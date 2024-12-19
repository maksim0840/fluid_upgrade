add_library(params STATIC runtime_params.cpp)
target_include_directories(params PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
