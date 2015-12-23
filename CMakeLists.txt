cmake_minimum_required(VERSION 3.2)
project(jeopardy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${jeopardy_SOURCE_DIR}/build)

set(SOURCE_FILES
        answer.cpp
        answer_screen.cpp
        base64.cpp
        category.cpp
        color.cpp
        game.cpp
        game_state.cpp
        invalid_json.cpp
        jeopardy_exception.cpp
        jeopardy_round.cpp
        main.cpp
        new_game.cpp
        player.cpp
        round_loader.cpp
        scoreboard.cpp
        setup_game.cpp
        websocket_server.cpp
        )

add_subdirectory(mediator)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if ( CMAKE_COMPILER_IS_GNUCC )
    set_property( TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -pedantic" )
endif ( CMAKE_COMPILER_IS_GNUCC )

target_link_libraries(${PROJECT_NAME} pthread boost_system boost_filesystem boost_regex mediator)
