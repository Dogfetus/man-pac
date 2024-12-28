//
// Created by dogfetus on 28.05.23.
//

#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <random>
#include "Entity.h"


class Enemy : public Entity{
public:
    Enemy(const float &start_posX, const float &start_posY, sf::RenderWindow &window, const Config &conf, sf::Texture &texture, sf::Color &);
    void draw() override;
    void move(float &TimeSinceLastFrame, const std::vector<Tile*> &map);
    void move(float &TimeSinceLastFrame, const float &x_destination, const float &y_destination);
    void regulateSpeed(bool player_is_invincible);
    Direction RandomDirection();



protected:
    sf::Color color;

    std::uniform_int_distribution<int> random_direction;
    std::random_device RandDev;
    std::mt19937_64 Gen;
    std::uniform_int_distribution<int> Num;

    sf::Clock animation;
    bool toggle_texture;

    sf::Clock dirClock;
    std::uniform_int_distribution<int> randomDirTime;

    sf::Clock change_direction;
};

#endif //GAME_ENEMY_H
