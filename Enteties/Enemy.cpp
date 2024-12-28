//
// Created by dogfetus on 28.05.23.
//

#include "Enemy.h"

Enemy::Enemy(const float &start_posX, const float &start_posY, sf::RenderWindow &window, const Config &conf, sf::Texture &texture, sf::Color &color) :
        Entity(start_posX, start_posY, window, conf),
        random_direction(0,3),
        randomDirTime(3,10),
        Gen(RandDev()),
        color(color){

    toggle_texture = false;
    dirClock.restart();

    scale = config.enemy.Scale;
    size = config.enemy.Size;
    currentSize = size * scale;
    points = config.enemy.Points;
    xspeed = config.enemy.Speed_x;
    yspeed = config.enemy.Speed_y;

    shape.setScale(scale, scale);
    shape.setTexture(texture);
    shape.setTextureRect(sf::IntRect(0,0,(int)size,(int)size));
    shape.setColor(color);
    shape.setPosition(start_posX,start_posY);
}


void Enemy::draw(){
    if (toggle_texture) { shape.setTextureRect(sf::IntRect(0,0,(int)size,(int)size));     }
    else                { shape.setTextureRect(sf::IntRect((int)size, 0, (int)size, (int)size)); }

    if(animation.getElapsedTime().asMilliseconds() > 200){
        toggle_texture = !toggle_texture;
        animation.restart();
    }

    shape.setPosition(x, y);
    window.draw(shape);
}

void Enemy::move(float &TimeSinceLastFrame, const std::vector<Tile*> &map){
    float next_x;
    float next_y;

    if (dirClock.getElapsedTime().asSeconds() >= (float)randomDirTime(Gen)){
        next_direction = RandomDirection();
        dirClock.restart();
    }

    switch (next_direction) {
        case Right: next_x = x + xspeed * TimeSinceLastFrame; next_y = y; break;
        case Left: next_x = x - xspeed * TimeSinceLastFrame; next_y = y; break;
        case Up: next_y = y - yspeed * TimeSinceLastFrame; next_x = x; break;
        case Down: next_y = y + yspeed * TimeSinceLastFrame; next_x = x; break;
        default: break;
    }

    if(!checkCollision(map, next_x, next_y, next_direction))
        moving_direction = next_direction;


    // stops ghosts from exiting level
    if (x + size >= (float)config.window.Width){
        moving_direction = Left;
        next_direction = Left;
    }
    if (x <= 0) {
        moving_direction = Right;
        next_direction = Right;
    }

    if (y + size >= (float)config.window.Height){
        moving_direction = Up;
        next_direction = Up;
    }

    if (y <= 0) {
        moving_direction = Down;
        next_direction = Down;
    }



    switch (moving_direction) {
        case Right:
            next_x = x + xspeed * TimeSinceLastFrame;
            if (!checkCollision(map, next_x, y, moving_direction))
                x += xspeed * TimeSinceLastFrame;
            else
                next_direction = RandomDirection();
            break;

        case Left:
            next_x = x - xspeed * TimeSinceLastFrame;
            if (!checkCollision(map, next_x, y, moving_direction))
                x -= xspeed * TimeSinceLastFrame;
            else
                next_direction = RandomDirection();
            break;

        case Up:
            next_y = y - yspeed * TimeSinceLastFrame;
            if (!checkCollision(map, x, next_y, moving_direction))
                y -= yspeed * TimeSinceLastFrame;
            else
                next_direction = RandomDirection();
            break;

        case Down:
            next_y = y + yspeed * TimeSinceLastFrame;
            if (!checkCollision(map, x, next_y, moving_direction))
                y += yspeed * TimeSinceLastFrame;
            else
                next_direction = RandomDirection();
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
    int rowUnderPlayer = (playerRow+1) <= maxRows ? ((playerRow+1) * columnsPerRow) : (playerRow * columnsPerRow); // 25 = max number of rows
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
}

void Enemy::move(float &TimeSinceLastFrame, const float &x_destination, const float &y_destination) {
    float Vec_x = x_destination - x;
    float Vec_y = y_destination - y;
    float Vec_length = sqrtf(Vec_x*Vec_x + Vec_y*Vec_y);

    x += Vec_x / Vec_length * xspeed * TimeSinceLastFrame;
    y += Vec_y / Vec_length * yspeed * TimeSinceLastFrame;

    x = std::max(x, 0.f);
    x = std::min(x, ((float)config.window.Width - size));
    y = std::max(y, 0.f);
    y = std::min(y, ((float)config.window.Height- size));

}


Enemy::Direction Enemy::RandomDirection(){
    switch (random_direction(Gen)) {
        case 0 : return Up;
        case 1 : return Down;
        case 2 : return Right;
        case 3 : return Left;
    }
    return None;
}

void Enemy::regulateSpeed(bool player_is_invincible) {
    if (player_is_invincible) {
        shape.setColor(sf::Color::Blue);
        xspeed = config.enemy.Speed_x / 2;
        yspeed = config.enemy.Speed_y /2;
    }

    else{
        shape.setColor(color);
        xspeed = config.enemy.Speed_x;
        yspeed = config.enemy.Speed_y;
    }
}