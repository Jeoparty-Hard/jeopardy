cmake_minimum_required(VERSION 3.2)
project(jeopardy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${jeopardy_SOURCE_DIR}/build)

set(SOURCE_FILES main.cpp)

add_subdirectory(mediator)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} pthread boost_system mediator)