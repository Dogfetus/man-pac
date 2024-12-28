//
// Created by dogfetus on 29.06.23.
//

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <initializer_list>

struct Element{
    int id;
    sf::Sprite sprite;
    sf::Texture texture;
    unsigned int height;
    unsigned int width;
};

class Menu {
public:
    explicit Menu(sf::RenderWindow &window) : paused(true), exit_(false), reset_(false), window(window), choice(1) { onceESC = onceDOWN = onceUP = true;}
    bool isShowing();
    void LoadFromFile(const std::initializer_list<std::string>& files);
    void draw();
    void show();
    void pickAnOption();
    bool reset();
    bool exit() const { return exit_; };
    bool play() const { return paused; };


private:

    sf::Sprite background;
    sf::Texture Background;
    sf::RenderWindow &window;

    std::vector<Element*> pauseMenu;

    bool paused;
    bool onceESC, onceUP, onceDOWN;
    bool reset_;
    bool exit_;

    int choice;
};

#endif //GAME_MENU_H
