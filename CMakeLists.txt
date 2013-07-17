cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)

project(kardio)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(src)

add_executable(kardio main.cxx)
qt5_use_modules(kardio Widgets)
target_link_libraries(kardio core)

install(TARGETS kardio RUNTIME DESTINATION bin)
