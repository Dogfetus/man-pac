//
// Created by dogfetus on 15.04.23.
//

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <SFML/Graphics.hpp>
#include <iostream>

class Config {
public:
    float default_tile_size = 39;

    //window
    struct {
        int FrameRateLimit = 60;
        int Width = 975;  // 1209
        int Height = 975; // 1092
        bool VSync = true;
        std::string Title = "Jesus knows you";
    } window;

    //player
    struct {
        int lives = 3;
        float Size = 52;
        float Scale = 0.7;
        float Speed_x = 200;
        float Speed_y = 200;
        float Speed = 200;
        float SpeedMultiplier = ((Speed_y+Speed_x)/2.f)*20;
        bool hitbox = true;
        sf::Color Color = sf::Color::Black;
    } player;

    //enemies
    struct {
        int Max = 40;
        float Points = 200;
        float Size = 39;
        float Scale = 1;
        float Speed_x = 180;
        float Speed_y = 180;
        bool Move = true;
        sf::Color Color = sf::Color::Red;
    } enemy;
};

#endif //GAME_CONFIG_H
