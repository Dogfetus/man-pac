//
// Created by dogfetus on 15.04.23.
//

#ifndef GAME_GAME_H
#define GAME_GAME_H
#include <SFML/Graphics.hpp>
#include "config.h"
#include "Enteties/player.h"
#include "Enteties/Enemy.h"
#include <list>
#include <mutex>
#include <condition_variable>
#include "Map/Map.h"
#include "Menu/Menu.h"

class Game{
public:
    Game();
    bool is_running();
    void reset();
    void stop();


private:
    void Game_functions();
    void Draw_score();
    void Retrieve_points();

    Config config;
    sf::RenderWindow window;
    sf::Texture player_texture;
    sf::Texture enemy_texture;

    Player player;
    std::list<Enemy*> enemy_list;
    std::vector<Entity*> points;
    std::vector<Entity*> boost;
    Map map;
    Menu menu;

    sf::Clock clock;
    std::mutex mutex1;
    sf::Thread Game_handler;
    std::condition_variable flag;

    sf::Texture numbers;
    std::vector<sf::Sprite> score;
    int Score;

    int num_of_loop = 0;
    bool level_cleared = false;
    bool hasDied = false;
    float pointMultiplier = 1;

};

#endif //GAME_GAME_H
