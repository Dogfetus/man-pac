#include <iostream>
#include "game.h"

int main() {

    Game game;

    while (game.is_running());

    game.stop();

    std::cout << "jesus " << std::endl;

    return 0;
}
