//
// Created by dogfetus on 15.04.23.
//

#include <SFML/Graphics.hpp>
#include "game.h"
#include <random>
#include <iostream>
#include <string>

Game::Game() :
    player(500, 500, window, config, player_texture),
    map(window, config),
    menu(window),
    Game_handler([this]{Game_functions();})
    {

        std::string image_folder = "Images/";
        window.create(sf::VideoMode(config.window.Width, config.window.Height), config.window.Title);
        clock.restart();
        Game_handler.launch();
        player_texture.loadFromFile ( image_folder + "packman.png" );
        enemy_texture.loadFromFile (image_folder +  "Ghost.png" );
//        map.LoadFromFile ("Map/Packmap.json", image_folder + "Map.png");
        map.LoadFromString("#########################"
                           "#o..........#..........o#"
                           "#.###.#####.#.#####.###.#"
                           "#.# #.#   #.#.#   #.# #.#"
                           "#.###.#####.#.#####.###.#"
                           "#.......................#"
                           "#.###.#.#########.#.###.#"
                           "#.....#.....#.....#.....#"
                           "#####.#####.#.#####.#####"
                           "    #.#...#.#.#...#.#    "
                           "    #.#.#...G...#.#.#    "
                           "#####.#.#.##D##.#.#.#####"
                           "........#.#GGG#.#........"
                           "#####.#.#.#####.#.#.#####"
                           "    #.#.....o.....#.#    "
                           "    #.#.#########.#.#    "
                           "#####.#.#...#...#.#.#####"
                           "#.........#.#.#.........#"
                           "#.###.#####.#.#####.###.#"
                           "#...#.......P.......#...#"
                           "###.#.#.##.###.##.#.#.###"
                           "#.....#.....#.....#.....#"
                           "#.#########.#.#########.#"
                           "#o.....................o#"
                           "#########################");
//map.LoadFromString("##########################           #           ## ### ##### # ##### ### ## ### ##### # ##### ### ## ### ##### # ##### ### ##                       ## ### # ######### # ### ##     #     #     #     ###### ##### # ##### #####    # #   # # #   # #        # # #   G   # # #    ##### # # ##D## # # #####        # #GGG# #        ##### # # ##### # # #####    # #           # #        # # ######### # #    ##### # #   #   # # ######         # # #         ## ### ##### # ##### ### ##   #       P    .  #   #### # # ## ### ## # # ####     #     #     #     ## ######### # ######### ##                       ##########################");
        menu.LoadFromFile({image_folder + "Title.png",
                               image_folder + "Play.png",
                               image_folder + "Reset.png",
                               image_folder + "Settings.png",
                               image_folder + "Exit.png"
        });
        window.setFramerateLimit(config.window.FrameRateLimit);
        Score = 0;
        player.setStartPos(map.GetPlayerSpawn());
        numbers.loadFromFile(image_folder + "numbers.png");
        Retrieve_points();
    }

bool Game::is_running() {

    float delta = clock.restart().asSeconds();

    num_of_loop++;
    //VSync
    window.setVerticalSyncEnabled(config.window.VSync);

    //interrupt
    sf::Event event{};

    while (window.pollEvent(event)) {
        //closes program if 'x' is hit
        if (event.type == sf::Event::Closed || menu.exit()){
            window.close();
            return false;
        }
    }

    //Black background
    window.clear(sf::Color::Black);

    if (level_cleared)
        menu.show();

    //if game is paused
    if (menu.isShowing()){
        menu.pickAnOption();
        menu.draw();

        if (menu.reset() || (menu.play() && level_cleared)){
            reset();
            level_cleared = false;
        }
    }

    // If game is not paused:
    else{
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5) || player.getLives() <= 0)
            reset();
        map.draw();

        mutex1.lock();
        for (auto point : points){
            point->draw();
        }

        for (auto bst : boost){
            bst->draw();
        }

        if (!player.isDead())
            player.move(delta, map.GetTiles());
        player.draw();

        for (auto enemy : enemy_list) {
            if(config.enemy.Move && !player.isDead())
                enemy->move(delta, map.GetTiles());
            else
                enemy->reset();
            enemy->regulateSpeed(player.isInvincible());
            enemy->draw();
        }
        mutex1.unlock();

        Draw_score();
        flag.notify_all();
    }

    //buffer swap: Flips which buffer is shown. Front / Back.
    window.display();

    if (num_of_loop >= 20){
        std::cout << 1.f/delta << std::endl;
        num_of_loop = 0;

    }

    return true;
}

void Game::Game_functions() {
    std::random_device RanDev;
    std::mt19937_64 generator(RanDev());
    std::uniform_int_distribution<int> random_width(0, config.window.Width-(int)config.enemy.Size);
    std::uniform_int_distribution<int> random_height(0, config.window.Height-(int)config.enemy.Size);
    std::uniform_int_distribution<int> random_color(0, 7);
    std::mutex flag_mutex;
    std::unique_lock<std::mutex> lock(flag_mutex);
    sf::Color color;

    while (window.isOpen()){
        flag.wait(lock);

        float spawnPosY;
        float spawnPosX;


        for (auto &pos : map.GetGhostSpawn()){

            if (enemy_list.size() >= map.GetGhostSpawn().size())
                continue;

            switch (random_color(generator)){

                case 0 : color = sf::Color::Yellow;     break;
                case 1 : color = sf::Color::Red;        break;
                case 2 : color = sf::Color::Green;      break;
                case 3 : color = sf::Color::Cyan;       break;
                case 4 : color = sf::Color::White;      break;
                case 5 :
                case 6 : color = sf::Color::Blue;       break;
                case 7 : color = sf::Color::Magenta;    break;
            }

            spawnPosX = pos.x;
            spawnPosY = pos.y;

            auto *enemy = new Enemy(spawnPosX,
                                    spawnPosY,
                                    window, config, enemy_texture, color);
            enemy_list.emplace_back(enemy);
        }


        for (auto enemy = enemy_list.begin(); enemy != enemy_list.end();) {
            if (player.intersects(*(*enemy)) &&
                config.player.hitbox            ){

                if (player.isInvincible()) {
                    mutex1.lock();
                    player.grow((*enemy)->getPoints() * pointMultiplier);
                    Score = (int)player.getPoints();
                    pointMultiplier *= 2;
                    delete *enemy;
                    enemy = enemy_list.erase(enemy);
                    mutex1.unlock();
                }

                else {
                    player.die();
                    enemy++;
                }

            }

            else {
                enemy++;
                if (!player.isInvincible())
                    pointMultiplier = 1;
            }
        }


        for (auto point = points.begin(); point != points.end();){
            mutex1.lock();
            if (player.intersects(*(*point))){
                player.grow((*point)->getPoints());
                Score = (int)player.getPoints();
                delete *point;
                point = points.erase(point);
            }else
                point++;
            mutex1.unlock();
        }


        for (auto bst = boost.begin(); bst != boost.end();){
            mutex1.lock();
            if (player.intersects(*(*bst))){
                player.grow((*bst)->getPoints());
                Score = (int)player.getPoints();
                delete *bst;
                bst = boost.erase(bst);
                player.turnInvincible();
            }else
                bst++;
            mutex1.unlock();
        }


        if (points.empty() && boost.empty())
            level_cleared = true;
    }
}

void Game::reset() {
//    map.LoadFromFile("Map/Packmap.json", "Images/Map.png");
//    map.LoadFromString(map.getMapAsString());
    for (auto &enemy : enemy_list)
        enemy->reset();
    player.reset();
    score.clear();
    Score = 0;
    Retrieve_points();
    level_cleared = false;
}

void Game::stop() {
    flag.notify_all();
    Game_handler.wait();
}

void Game::Draw_score(){
    std::string _score = std::to_string(Score);
    float posx = 10;
    float posy = 2;
    int num_width = 221;
    int num_height = 406;
    int scnd_row_offset = 59;

    for (int i = 0; i < _score.size(); i++){
        if (_score.size() > score.size()){
            sf::Sprite sprite;
            sprite.setTexture(numbers);
            sprite.setScale(.1,.1);
            score.emplace_back(sprite);
        }

        switch (_score[i]) {
            case '0': score[i].setTextureRect(sf::IntRect(0, 0, num_width, num_height));                                        break;
            case '1': score[i].setTextureRect(sf::IntRect(num_width, 0, num_width, num_height));                                break;
            case '2': score[i].setTextureRect(sf::IntRect(num_width*2, 0, num_width, num_height));                              break;
            case '3': score[i].setTextureRect(sf::IntRect(num_width*3, 0, num_width, num_height));                              break;
            case '4': score[i].setTextureRect(sf::IntRect(num_width*4, 0, num_width, num_height));                              break;
            case '5': score[i].setTextureRect(sf::IntRect(0, num_height + scnd_row_offset, num_width, num_height));             break;
            case '6': score[i].setTextureRect(sf::IntRect(num_width, num_height + scnd_row_offset, num_width, num_height));     break;
            case '7': score[i].setTextureRect(sf::IntRect(num_width*2, num_height + scnd_row_offset, num_width, num_height));   break;
            case '8': score[i].setTextureRect(sf::IntRect(num_width*3, num_height + scnd_row_offset, num_width, num_height));   break;
            case '9': score[i].setTextureRect(sf::IntRect(num_width*4, num_height + scnd_row_offset, num_width, num_height));   break;
        }

        score[i].setPosition(posx + (float)(num_width * i * 0.1), posy);
        window.draw(score[i]);
    }
}

void Game::Retrieve_points() {
    // remove previous data
    for (auto point : points){
        delete point;
    }
    points.clear();

    for (auto fruit : boost){
        delete fruit;
    }
    boost.clear();

    // points are given a fixed size
    for (auto& point : map.GetPointLocations()){
        float pointSize = 9;
        auto* entty = new Entity(point.x - pointSize/2, point.y - pointSize/2, window, config); // - 5 to place in middle of tile
        entty->setPoints(10);
        entty->setSize(pointSize);
        entty->setColor(sf::Color(255,225,154));
        points.emplace_back(entty);
    }

    for (auto& point : map.GetBoostLocations()){
        float boostSize = 20;
        auto* entty = new Entity(point.x - boostSize/2, point.y - boostSize/2, window, config); // - 5 to place in middle of tile
        entty->setPoints(50);
        entty->setSize(boostSize);
        entty->setColor(sf::Color(255,225,154));
        boost.emplace_back(entty);
    }
}