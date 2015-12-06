cmake_minimum_required(VERSION 3.2)
project(jeopardy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${jeopardy_SOURCE_DIR}/build)

set(SOURCE_FILES main.cpp answer.cpp category.cpp jeopardy_round.cpp invalid_json.cpp game.cpp websocket_server.cpp player.cpp base64.cpp game_state.cpp new_game.cpp jeopardy_exception.cpp)

add_subdirectory(mediator)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} pthread boost_system boost_filesystem boost_regex mediator)