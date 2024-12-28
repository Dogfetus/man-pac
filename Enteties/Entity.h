//
// Created by dogfetus on 15.04.23.
//

#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "../config.h"
#include <SFML/Graphics.hpp>
#include "../Map/Map.h"

class Entity{
protected:
    enum Direction{
        Right,
        Left,
        Up,
        Down,
        None
    };

    sf::RenderWindow &window;
    sf::Sprite shape;
    sf::CircleShape circle;
    const Config &config;

    float size = 0;
    float scale;
    float currentSize;
    float points;

    float x;
    float xspeed = 0;

    float y;
    float yspeed = 0;

    float startPosx;
    float startPosy;

    Direction next_direction;
    Direction moving_direction;

public:
    Entity(const float &start_posX, const float &start_posY, sf::RenderWindow &window, const Config &conf);

    void setStartPos(sf::Vector2f startpos);
    void setSize(float _size);
    void setPoints(float pts);
    void setColor(sf::Color clr);
    bool intersects(Entity& ent) const;

    virtual bool checkCollision(const std::vector<Tile*> &tiles, float x1, float y1, Direction dir);
    virtual void draw();
    virtual void reset();

    virtual float getSize() const   { return size;    }
    float getX() const              { return x;       }
    float getY() const              { return y;       }
    float getPoints() const         { return points;  }



};


#endif //GAME_ENTITY_H
