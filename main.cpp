#include "classes.h"

vector<vector<int>>level={          // Definicja mapy
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

sf::Clock gameClock; // Zegar gry (aby gra nie

/*
bool isColliding(float posX, float posY){ // Sprawdzanie kolizji
    int tileX=static_cast<int>(posX/blockSize);
    int tileY=static_cast<int>(posY/blockSize);
    return (tileX<0 || tileX>=mapWidth || tileY<0 || tileY>=mapHeight || map[tileY][tileX]==1);
}
*/

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) { // Funkcja main bez uruchamiania konsoli

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Render Engine Project"); // Definicja okna

    window.setFramerateLimit(60); // Limit klatek ustawiony na 60


    sf::Image icon; // } Ikona programu
    icon.loadFromFile("icon.png"); // }
    window.setIcon(64, 64, icon.getPixelsPtr());  // }


    Map map(48.0f, level); // Zadeklarowanie mapy
    Player player(75, 75); // Zadeklarowanie gracza, ustawienie startowej pozycji na 50,50

    while(window.isOpen()){ // Gra
        sf::Event event;

        float gameTime=gameClock.restart().asSeconds(); // Zegar gry


        while(window.pollEvent(event)){ // Zamkniecie gry przy zamknieciu okna
            if(event.type==sf::Event::Closed)
                window.close();
            if((event.type==sf::Event::KeyPressed) && (event.key.code==sf::Keyboard::Escape))
                window.close();
        }

        player.update(gameTime, map);
//        player.update(gameTime);

        window.clear();
        map.draw(window);

        player.draw(window);

        window.display();
    }

    return 0;
}
