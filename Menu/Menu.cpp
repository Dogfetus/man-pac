//
// Created by dogfetus on 29.06.23.
//

#include "Menu.h"
#include <initializer_list>

void Menu::LoadFromFile(const std::initializer_list<std::string>& files){

    int id = 0;
    Element* previous = nullptr; // might move this to draw();

    for (auto& file : files) {
        if (id == -1){
            Background.loadFromFile(file);
            Background.setRepeated(true);
            background.setTexture(Background);
            id ++;
        } else{
            auto element = new Element;
            element->id = id++;
            element->texture.loadFromFile(file);
            element->width = element->texture.getSize().x;
            element->height = element->texture.getSize().y;
            element->sprite.setTexture(element->texture);

            if (previous)
                element->sprite.setPosition(((float)window.getSize().x/2.f) - (float)element->width/2.f,previous->sprite.getPosition().y + (float)previous->height);
            else
                element->sprite.setPosition(((float)window.getSize().x/2.f) - (float)element->width/2.f,0);

            pauseMenu.emplace_back(element);

            previous = element;

        }
    }
}


bool Menu::isShowing() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        if (onceESC)
            paused = !paused;
        onceESC = false;
    } else
        onceESC = true;
    return paused;
}


void Menu::draw() {
    for (auto element: pauseMenu) {
        // makes options yellow when choosing
        if (choice == element->id)
            element->sprite.setColor(sf::Color(255,255,0,255));
        else if (element->id != 0)
            element->sprite.setColor(sf::Color(255,255,255,100));
        window.draw(element->sprite);
    }
}

void Menu::show() {
    paused = true;
}

void Menu::pickAnOption() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
        if (onceDOWN)
            choice ++;
        onceDOWN = false;
    } else
        onceDOWN = true;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
        if (onceUP)
            choice --;
        onceUP = false;
    } else
        onceUP = true;

    if (choice >= pauseMenu.size())
        choice = (int)pauseMenu.size()-1;
    if (choice < 1)
        choice = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
        switch (choice) {
            case 1:
                paused = false;
                break;
            case 2:
                reset_ = true;
                break;
            case 3:
                paused = false;
                break;
            case 4:
            default:
                exit_ = true;
                break;
        }
}


bool Menu::reset() {
    bool value = reset_;
    reset_ = false;
    return value;
}
