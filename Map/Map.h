//
// Created by dogfetus on 23.06.23.
//

#ifndef GAME_MAP_H
#define GAME_MAP_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "../config.h"
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"


typedef struct {
    float x;
    float y;
    float height;
    float width;

} Object;

typedef struct {
    sf::Sprite shape;

    float x;
    float y;
    float height;
    float width;

    bool Collision;
    bool isDoor;
} Tile;

class Map{
public:
    explicit Map(sf::RenderWindow &window, Config &config) : window(window), config(config) {}
    bool LoadFromFile(const std::string &json, const std::string &picture);
    bool LoadFromString(const std::string &mapstring);
    void draw();

    std::string GetMapAsString()                    { return map_as_string; };
    std::vector<sf::Vector2f> GetGhostSpawn()       { return ghostPos;      };
    sf::Vector2f GetPlayerSpawn()                   { return playerPos;     };
    std::vector<sf::Sprite*> GetSprites()           { return mapSprites;    };
    std::vector<Object*> GetObjects()               { return mapObjects;    };
    std::vector<Tile*> GetTiles()                   { return map_vector;    };
    std::vector<sf::Vector2f> GetPointLocations()   { return points;        };
    std::vector<sf::Vector2f> GetBoostLocations()   { return boosts;        };
    std::vector<sf::Vector2f> GetFruitLocations()   { return fruits;        };

protected:

    sf::Vector2f playerPos;
    std::vector<sf::Vector2f> ghostPos;

    std::vector<sf::Vector2f> points;
    std::vector<sf::Vector2f> boosts;
    std::vector<sf::Vector2f> fruits;

    std::string map_as_string;
    std::vector<Tile*> map_vector;

    std::vector<Object*> mapObjects;
    sf::Texture mapTexture;
    std::vector<sf::Sprite*> mapSprites;
    sf::RenderWindow &window;
    Config &config;
};

#endif //GAME_MAP_H
