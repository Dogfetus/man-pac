cmake_minimum_required(VERSION 3.21)
project(Game)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

set(SOURCE_FILES
        main.cpp
        game.cpp
        game.h
        Enteties/Entity.cpp
        Enteties/Entity.h
        Enteties/player.cpp
        Enteties/player.h
        config.h
        Enteties/Enemy.cpp
        Enteties/Enemy.h
        Map/Map.cpp
        Map/Map.h Map/Map.h Menu/Menu.cpp Menu/Menu.h)
add_executable(Game ${SOURCE_FILES})
target_link_libraries(Game PRIVATE sfml-system sfml-graphics sfml-window)
