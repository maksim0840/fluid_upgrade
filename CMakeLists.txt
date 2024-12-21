cmake_minimum_required(VERSION 3.10)
project(Fluid)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_COMPILER "g++-13")

add_subdirectory(types)
add_subdirectory(start_values)
add_subdirectory(params)
add_subdirectory(emulator)

add_executable(fluid fluid.cpp)
target_link_libraries(fluid PRIVATE params emulator)
target_include_directories(fluid PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Возможность добавить параметр -DTYPES= через cmake
if (DEFINED TYPES)
    target_compile_definitions(fluid PRIVATE -DTYPES="${TYPES}")
endif()
