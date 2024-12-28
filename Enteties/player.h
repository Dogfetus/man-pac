//
// Created by dogfetus on 15.04.23.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Entity.h"

class Player : public Entity{
public:
    Player(const float &start_posX, const float &start_posY, sf::RenderWindow &window, Config &conf, sf::Texture &texture);

    void draw() override;
    void move(float &TimeSinceLastFrame, const std::vector<Tile*> &map);
    void grow(float points);
    void reset() override;
    void die();
    void respawn();
    void turnInvincible();

    float getSize() const override;

    bool checkCollision(const std::vector<Tile*> &tiles, float x1, float y1, Direction dir) override;
    bool isInvincible();
    bool isDead()  const        { return dead; };
    int getLives() const        { return lives; };

protected:

    sf::Clock animation;
    sf::Clock invincibleTimer;
    bool start_animation;
    float multiplier;

    bool invincible;
    int lives;
    bool dead;
    bool deathAnimation;
    short deadimation;

};


#endif //GAME_PLAYER_H
