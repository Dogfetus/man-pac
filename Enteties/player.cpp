//
// Created by dogfetus on 15.04.23.
//

#include "player.h"
#include <cmath>

Player::Player(const float &start_posX, const float &start_posY, sf::RenderWindow &window, Config &conf, sf::Texture &texture) :
    Entity(start_posX, start_posY, window, conf),
    start_animation(false),
    deathAnimation(false),
    multiplier(1),
    invincible(false),
    deadimation(0),
    lives(config.player.lives),
    dead(false){

    scale = config.player.Scale;
    size = config.player.Size;
    xspeed = config.player.Speed_x;
    yspeed = config.player.Speed_y;
    currentSize = size*scale;

    moving_direction = None;
    next_direction = None;

    shape.setTexture(texture);
    shape.setTextureRect(sf::IntRect(0, 0, 52, 52));
    shape.setScale(scale,scale);

    animation.restart();
}

void Player::draw() {
    if (animation.getElapsedTime().asMilliseconds() >= 250){
        start_animation = !start_animation;
        animation.restart();
    }


    if (dead){
        if (deathAnimation == start_animation){
            deathAnimation = !deathAnimation;
            deadimation++;
            switch (deadimation) {
                case 1: shape.setTextureRect(sf::IntRect (4*(int)config.player.Size+8, 0, (int)config.player.Size, (int)config.player.Size) ); break;
                case 2: shape.setTextureRect(sf::IntRect (5*(int)config.player.Size+10, 0, (int)config.player.Size, (int)config.player.Size) ); break;
                case 3: shape.setTextureRect(sf::IntRect (4*(int)config.player.Size+8, (int)config.player.Size, (int)config.player.Size, (int)config.player.Size) ); break;
                default: shape.setTextureRect(sf::IntRect (5*(int)config.player.Size+10, (int)config.player.Size, (int)config.player.Size, (int)config.player.Size) ); break;
            }
            if (deadimation > 4)
                respawn();
         }
    }

    else{
        switch (moving_direction) {
            case Up:
                shape.setTextureRect(sf::IntRect(0, 54, (int)size, (int)size));
                if (start_animation)
                    shape.setTextureRect(sf::IntRect(107, 54, (int)size, (int)size));
                break;

            case Down:
                shape.setTextureRect(sf::IntRect(53, 0, (int)size, (int)size));
                if (start_animation)
                    shape.setTextureRect(sf::IntRect(161, 0, (int)size, (int)size));
                break;

            case Right:
                shape.setTextureRect(sf::IntRect(0, 0, (int)size, (int)size));
                if (start_animation)
                    shape.setTextureRect(sf::IntRect(107, 0, (int)size, (int)size));
                break;

            case Left:
                shape.setTextureRect(sf::IntRect(53, 54, (int)size, (int)size));
                if (start_animation)
                    shape.setTextureRect(sf::IntRect(161, 54, (int)size, (int)size));
                break;
            default:
                break;
        }
    }


    shape.setPosition(x, y);
    window.draw(shape);

}

void Player::move(float &TimeSinceLastFrame, const std::vector<Tile*> &map) {
    float next_x;
    float next_y;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::D)         ||
    next_direction == Right) {

        next_direction = Right;
        next_x = x + (xspeed * TimeSinceLastFrame);
        if (!checkCollision(map, next_x, y, next_direction))
            moving_direction = next_direction;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)  ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::A)         ||
    next_direction == Left) {

        next_direction = Left;
        next_x = x - (xspeed * TimeSinceLastFrame);
        if (!checkCollision(map, next_x, y, next_direction))
            moving_direction = next_direction;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)    ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::W)         ||
    next_direction == Up) {

        next_direction = Up;
        next_y = y - (yspeed * TimeSinceLastFrame);
        if (!checkCollision(map,x,next_y, next_direction))
            moving_direction = next_direction;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)  ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::S)         ||
    next_direction == Down){

        next_direction = Down;
        next_y = y + (yspeed * TimeSinceLastFrame);
        if (!checkCollision(map, x, next_y, next_direction))
            moving_direction = next_direction;
    }


    switch (moving_direction) {
        case Up:
            next_y = y - (yspeed * TimeSinceLastFrame);
            if (!checkCollision(map,x,next_y, moving_direction))
                y -= yspeed * TimeSinceLastFrame;
            else
                moving_direction = None;
            break;

        case Down:
            next_y = y + (yspeed * TimeSinceLastFrame);
            if (!checkCollision(map,x,next_y, moving_direction))
                y += yspeed * TimeSinceLastFrame;
            else
                moving_direction = None;
            break;

        case Right:
            next_x = x + (xspeed * TimeSinceLastFrame);
            if (!checkCollision(map,next_x,y, moving_direction))
                x += xspeed * TimeSinceLastFrame;
            else
                moving_direction = None;
            break;

        case Left:
            next_x = x - (xspeed * TimeSinceLastFrame);
            if (!checkCollision(map,next_x,y, moving_direction))
                x -= xspeed * TimeSinceLastFrame;
            else
                moving_direction = None;
            break;

        default:
            break;

    }



    // makes it easier to enter holes
    int columnsPerRow = static_cast<int>((float)config.window.Width/config.default_tile_size);
    int playerRow = static_cast<int>(y / config.default_tile_size);
    int playerColum = static_cast<int>(x / config.default_tile_size);
    int maxRows = static_cast<int>((float)config.window.Height/config.default_tile_size - 1);

    int rowAbovePlayer = (playerRow-1) >= 0 ? ((playerRow-1) * columnsPerRow) : (playerRow * columnsPerRow);
    int rowUnderPlayer = (playerRow+1) < maxRows ? ((playerRow+1) * columnsPerRow) : (playerRow * columnsPerRow); // 25 = max number of rows
    int columnLeftOfPlayer = (playerColum - 1) >= 0 ? (playerColum - 1) : (playerColum);
    int columnRightOfPlayer = (playerColum + 1) <= maxRows ? (playerColum + 1) : (playerColum);

    Tile* tileAbovePlayer = map[rowAbovePlayer + playerColum];
    Tile* tileUnderPlayer = map[rowUnderPlayer + playerColum];
    Tile* tileRightOfPlayer = map[(playerRow * columnsPerRow) + columnRightOfPlayer];
    Tile* tileLeftOfPlayer = map[(playerRow * columnsPerRow) + columnLeftOfPlayer];

    if (moving_direction != next_direction){
        switch (next_direction) {
            case Up:
                if (!tileAbovePlayer->Collision &&
                    tileAbovePlayer->x + xspeed*TimeSinceLastFrame > x)
                    x = tileAbovePlayer->x;
                break;

            case Down:
                if (!tileUnderPlayer->Collision &&
                    tileUnderPlayer->x + xspeed*TimeSinceLastFrame > x)
                    x = tileUnderPlayer->x;
                break;

            case Left:
                if (!tileLeftOfPlayer->Collision &&
                    tileLeftOfPlayer->y + yspeed*TimeSinceLastFrame > y)
                    y = tileLeftOfPlayer->y;
                break;

            case Right:
                if (!tileRightOfPlayer->Collision &&
                    tileRightOfPlayer->y + yspeed*TimeSinceLastFrame > y)
                    y = tileRightOfPlayer->y;
                break;

            default: break;
        }
    }


    // to teleport to other side of screen
    if (x + size*scale <= 0)
        x = (float)config.window.Width;
    if (x > (float)config.window.Width)
        x = -size*scale;

    if (y + size*scale <= 0)
        y = (float)config.window.Height;
    if (y > (float)config.window.Height)
        y = -size*scale;

}

void Player::grow(float points){
    this->points+=points * multiplier;
}


float Player::getSize() const {
    return size * scale;
}


void Player::reset() {
    Entity::reset();
    respawn();
    xspeed = yspeed = config.player.Speed;
    points = 0;
    lives = 3;
}


void Player::die() {
    dead = true;
}


bool Player::checkCollision(const std::vector<Tile *> &tiles, float x1, float y1, Entity::Direction dir) {
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


        if (tiles[(row_num * tiles_per_row) + column_num]->Collision ||
            tiles[(row_num * tiles_per_row) + column_num]->isDoor){
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


}


void Player::respawn(){
    Entity::reset();
    lives--;
    deadimation = 0;
    moving_direction = None;
    next_direction = None;
    dead = false;
    shape.setTextureRect(sf::IntRect(0,0,(int)size, (int)size));
}


void Player::turnInvincible() {
    invincibleTimer.restart();
    invincible = true;
}


bool Player::isInvincible() {
    if (invincibleTimer.getElapsedTime().asSeconds() > 8)
        invincible = false;

    return  invincible;
}
