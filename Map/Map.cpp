//
// Created by dogfetus on 23.06.23.
//

#include <fstream>
#include <SFML/Graphics.hpp>
#include "Map.h"




bool Map::LoadFromFile(const std::string &filename, const std::string &picture) {
    int row = 0;
    int column = 0;

    if(!mapTexture.loadFromFile(picture)) {
        return false;
    }

    // deletes previous info
    for (auto sprite : mapSprites){
        delete sprite;
    }

    for (auto tile : map_vector){
        delete tile;
    }
    map_vector.clear();

    for (auto object : mapObjects){
        delete object;
    }
    mapSprites.clear();
    mapObjects.clear();

    // Will contain the data we read in
    rapidjson::Document root;

    // Stream used for reading the data file. The data file has been saved as JSON in Tiled
    std::ifstream ifs(filename);

    // Convert the stream to the rapidjson stream type
    rapidjson::IStreamWrapper isw(ifs);

    // Read data from file into root object
    root.ParseStream(isw);

    if (root.HasParseError())
        return false;



    // Retrieve background layer
    for (auto &num :  root["layers"][0]["data"].GetArray()){

        auto sprite = new sf::Sprite;
        sprite->setTexture(mapTexture);

        int tilewidth = root["tilewidth"].GetInt();
        int tileheight = root["tileheight"].GetInt();

        sprite->setPosition((float)(column*tilewidth), (float)(row*tileheight));
        column++;

        switch (num.GetInt()) {
            case 1: sprite->setTextureRect(sf::IntRect(0,0,tilewidth,tileheight));                      break;
            case 2: sprite->setTextureRect(sf::IntRect(tilewidth,0,tilewidth,tileheight));              break;
            case 3: sprite->setTextureRect(sf::IntRect(tilewidth*2,0,tilewidth,tileheight));            break;
            case 4: sprite->setTextureRect(sf::IntRect(tilewidth*3,0,tilewidth,tileheight));            break;
            case 5: sprite->setTextureRect(sf::IntRect(tilewidth*4,0,tilewidth,tileheight));            break;
            case 6: sprite->setTextureRect(sf::IntRect(0,tileheight,tilewidth,tileheight));             break;
            case 7: sprite->setTextureRect(sf::IntRect(tilewidth,tileheight,tilewidth,tileheight));     break;
            case 8: sprite->setTextureRect(sf::IntRect(tilewidth*2,tileheight,tilewidth,tileheight));   break;
            case 9: sprite->setTextureRect(sf::IntRect(tilewidth*3,tileheight,tilewidth,tileheight));   break;
            case 10: sprite->setTextureRect(sf::IntRect(tilewidth*4,tileheight,tilewidth,tileheight));  break;
        }

        if (column >= root["width"].GetInt()){
            row++;
            column = 0;
        }

        mapSprites.emplace_back(sprite);
    }


    // retrieve objects:
    int layers = 0;
//    int id = 0;
    for (auto &Layer : root["layers"].GetArray()){
        if (layers == 0){
            layers ++;
            continue;
        }

        for (auto &object : Layer["objects"].GetArray()){
            auto Object = new ::Object;

            Object->x = object["x"].GetFloat();
            Object->y = object["y"].GetFloat();
            Object->height = object["height"].GetFloat();
            Object->width = object["width"].GetFloat();
//            Object->id = id++;

            mapObjects.emplace_back(Object);
        }
    }

    return true;
}

bool Map::LoadFromString(const std::string &map) {
    map_as_string = map;

    if(!mapTexture.loadFromFile("Images/PacMap.png"))
        return false;

    // remove previous data
    for (auto tile : map_vector){
        delete tile;
    }
    map_vector.clear();

    for (auto tile : mapSprites){
        delete tile;
    }
    mapSprites.clear();

    for (auto obj : mapObjects){
        delete obj;
    }
    mapObjects.clear();
    boosts.clear();
    points.clear();
    fruits.clear();


    float width = 0;
    float height = 0;
    int tileSize = static_cast<int>(config.default_tile_size);
    sf::Vector2f temp; // used to make ghosts, points, fruits and boosts

    for (auto &c : map){
        auto *tile = new Tile ;

        tile->shape.setTexture(mapTexture);
        tile->shape.setTextureRect(sf::IntRect(7*tileSize, tileSize, tileSize, tileSize));
        tile->shape.setPosition(width, height);
        tile->x = width;
        tile->y = height;
        tile->height = tile->width = config.default_tile_size;

        switch (c) {
            case '#':
                tile->Collision = true;
                tile->isDoor = false;
                break;

            case 'G':
                temp.x = tile->x;
                temp.y = tile->y;
                ghostPos.emplace_back(temp);
                tile->Collision = tile->isDoor = false;
                break;

            case 'P':
                playerPos.x = width;
                playerPos.y = height;
                tile->Collision = tile->isDoor = false;
                break;

            case 'o':
                temp.x = tile->x + ( tile->width / 2); // middle of the tile
                temp.y = tile->y + (tile->height / 2); // middle of the tile
                boosts.emplace_back(temp);
                tile->Collision = tile->isDoor = false;
                break;

            case 'D':
                tile->isDoor = true;
                tile->shape.setTextureRect(sf::IntRect(7*tileSize, 0, tileSize, tileSize));
                tile->Collision = false;
                break;

            case '.':
                temp.x = tile->x + ( tile->width / 2); // middle of the tile
                temp.y = tile->y + (tile->height / 2); // middle of the tile
                points.emplace_back(temp);
                tile->Collision = tile->isDoor = false;
                break;

            default :
                tile->Collision = tile->isDoor = false;
                break;


        }

        map_vector.emplace_back(tile);
        width += config.default_tile_size;

        if (width >= (float)config.window.Width){
            width = 0;
            height += config.default_tile_size;
        }
    }


    std::reverse(ghostPos.begin(), ghostPos.end());


    for (auto tile : map_vector) {
        if (!tile->Collision)
            continue;

        int closeTiles = 0;

        auto columnsPerRow = static_cast<short>((float)config.window.Width/config.default_tile_size);   // 25
        auto maxRows = static_cast<short>((float)config.window.Height/config.default_tile_size);        // 25
        auto tileRow = static_cast<short>(tile->y / config.default_tile_size);
        auto tileColum = static_cast<short>(tile->x / config.default_tile_size);

        // Avoid overflow
        int rowAbove = (tileRow-1) >= 0 ? ((tileRow-1) * columnsPerRow) : (tileRow * columnsPerRow);
        int rowUnder = (tileRow+1) < maxRows ? ((tileRow+1) * columnsPerRow) : (tileRow * columnsPerRow); // 25 = max number of rows
        int columnLeft = (tileColum - 1) >= 0 ? (tileColum - 1) : (tileColum);
        int columnRight = (tileColum + 1) < maxRows ? (tileColum + 1) : (tileColum);

        // find tiles around main tile (may be the same)
        Tile* tileAbove = map_vector[rowAbove + tileColum];
        Tile* tileUnder = map_vector[rowUnder + tileColum];
        Tile* tileRight = map_vector[(tileRow * columnsPerRow) + columnRight];
        Tile* tileLeft = map_vector[(tileRow * columnsPerRow) + columnLeft];

        // to avoid overflow, tile around mid tile, might be set to mid tile. These will not be counted
        if (tileAbove->Collision && tile != tileAbove)
            closeTiles += 1;
        if (tileUnder->Collision && tile != tileUnder)
            closeTiles += 2;
        if (tileRight->Collision && tile != tileRight)
            closeTiles += 4;
        if (tileLeft->Collision && tile != tileLeft)
            closeTiles += 8;

        switch (closeTiles) {
            case 1: tile->shape.setTextureRect(sf::IntRect (tileSize, 0, tileSize, tileSize) ); break;
            case 2: tile->shape.setTextureRect(sf::IntRect (2*tileSize, 0, tileSize, tileSize) ); break;
            case 3: tile->shape.setTextureRect(sf::IntRect (8*tileSize, 0, tileSize, tileSize) ); break;
            case 4: tile->shape.setTextureRect(sf::IntRect (3*tileSize, 0, tileSize, tileSize) ); break;
            case 5: tile->shape.setTextureRect(sf::IntRect (6*tileSize, 0, tileSize, tileSize) ); break;
            case 6: tile->shape.setTextureRect(sf::IntRect (tileSize, tileSize, tileSize, tileSize) ); break;
            case 7: tile->shape.setTextureRect(sf::IntRect (6*tileSize, tileSize, tileSize, tileSize) ); break;
            case 8: tile->shape.setTextureRect(sf::IntRect (4*tileSize, 0, tileSize, tileSize) ); break;
            case 9: tile->shape.setTextureRect(sf::IntRect (5*tileSize, 0, tileSize, tileSize) ); break;
            case 10: tile->shape.setTextureRect(sf::IntRect (0, tileSize, tileSize, tileSize) ); break;
            case 11: tile->shape.setTextureRect(sf::IntRect (3*tileSize, tileSize, tileSize, tileSize) ); break;
            case 12: tile->shape.setTextureRect(sf::IntRect (8*tileSize, tileSize, tileSize, tileSize) ); break;
            case 13: tile->shape.setTextureRect(sf::IntRect (4*tileSize, tileSize, tileSize, tileSize) ); break;
            case 14: tile->shape.setTextureRect(sf::IntRect (5*tileSize, tileSize, tileSize, tileSize) ); break;
            case 15: tile->shape.setTextureRect(sf::IntRect (2*tileSize, tileSize, tileSize, tileSize) ); break;
            default: tile->shape.setTextureRect(sf::IntRect (0, 0, tileSize, tileSize) ); break;
        }
    }

    return true;
}

void Map::draw(){
    if (!mapSprites.empty()){
        for (auto &sprite : mapSprites) {
            window.draw(*sprite);
        }
    }

    else if (!map_vector.empty()){
        for (auto &tile : map_vector){
            window.draw(tile->shape);
        }
    }
}
