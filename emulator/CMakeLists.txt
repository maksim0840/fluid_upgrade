add_library(emulator INTERFACE)
target_link_libraries(emulator INTERFACE types start_values)
target_include_directories(emulator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})