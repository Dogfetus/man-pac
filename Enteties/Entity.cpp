//
// Created by dogfetus on 15.04.23.
//

#include "Entity.h"
#include <cmath>

Entity::Entity(const float &x, const float &y, sf::RenderWindow &window, const Config &conf):
        x(x)            ,
        y(y)            ,
        window(window)  ,
        config(conf)    ,
        startPosx(x)    ,
        startPosy(y)    ,
        scale(1){

        currentSize = size*scale;
        circle.setPosition(x, y);
        points = 0;
        next_direction = None;
        moving_direction = Right;
}

void Entity::draw() {
    window.draw(circle);
}

bool Entity::intersects(Entity& ent) const{
    float xdistance = x - ent.x;
    float ydisctance = y - ent.y;

    if (    xdistance   <=  ent.currentSize &&
            ydisctance  <=  ent.currentSize &&
            xdistance   >=  -currentSize    &&
            ydisctance  >=  -currentSize    ){
        return true;
    }

    return false;
}

void Entity::setSize(float _size){
    size = _size;
    circle.setRadius(_size/2);
}

void Entity::setPoints(float pts){
     points = pts;
}

void Entity::setColor(sf::Color clr) {
    circle.setFillColor(clr);
}



void Entity::reset() {
    x = startPosx;
    y = startPosy;
}

bool Entity::checkCollision(const std::vector<Tile*> &tiles, float x1, float y1, Direction dir){

    const short overlapping_tiles = 4; // max num of tiles man-pac overlaps

    int row_num;
    int column_num;

    auto tiles_per_row = static_cast<short>((float) config.window.Width / config.default_tile_size);    // 26
    auto total_rows = static_cast<short>(((float) config.window.Height / config.default_tile_size)) - 1; // -1 since it starts at row 0 (not row 1)

    for (short i = 0; i < overlapping_tiles; i++) {
        switch (i) {
            case 0: // top left tile
                row_num = floor((double) y1 / config.default_tile_size);
                column_num = floor((double) x1 / config.default_tile_size);
                break;


            case 1: // top right tile
                row_num = floor((double) y1 / config.default_tile_size);
                column_num = ceil((double) x1 / config.default_tile_size);
                break;


            case 2: // bottom left tile
                row_num = ceil((double) y1 / config.default_tile_size);
                column_num = floor((double) x1 / config.default_tile_size);
                break;


            case 3: // bottom right tile
                row_num = ceil((double) y1 / config.default_tile_size);
                column_num = ceil((double) x1 / config.default_tile_size);
                break;

            default:
                std::cout << "suspicious behaviour!" << std::endl;
        }


        if (row_num < 0)
            row_num = 0;
        else if (row_num >= total_rows)
            row_num = total_rows;


        if (column_num < 0)
            column_num = 0;
        else if (column_num >= tiles_per_row - 1)
            column_num = tiles_per_row - 1;


        if (tiles[(row_num * tiles_per_row) + column_num]->Collision){
            switch (dir) {
                case Right:     x = tiles[(row_num * tiles_per_row) + column_num - 1]->x;   break;
                case Left:      x = tiles[(row_num * tiles_per_row) + column_num + 1]->x;   break;
                case Up:        y = tiles[((row_num + 1) * tiles_per_row) + column_num]->y; break;
                case Down:      y = tiles[((row_num - 1) * tiles_per_row) + column_num]->y; break;
                default:
                    break;
            }
            return true;
        }
    }

    return false;

    //    for (auto &tile : tiles){
//        float xdistance = x1 - tile->x;
//        float ydisctance = y1 - tile->y;
//
//        if (    xdistance   <=  tile->width    &&
//                ydisctance  <=  tile->height   &&
//                xdistance   >=  -currentSize   &&
//                ydisctance  >=  -currentSize   &&
//                tile->Collision)    {
//            return true;
//        }
//    }
//    return false;

}



void Entity::setStartPos(sf::Vector2f startpos) {
    startPosx = startpos.x;
    startPosy = startpos.y;
    x = startPosx;
    y = startPosy;
}