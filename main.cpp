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

sf::Clock gameClock; // Zegar gry (aby gra dzialala niezaleznie od ilosci klatek)

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) { // Funkcja main bez uruchamiania konsoli

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Render Engine Project"); // Definicja okna

    window.setFramerateLimit(60); // Limit klatek ustawiony na 60


    sf::Image icon; // } Ikona programu
    icon.loadFromFile("icon.png"); // }
    window.setIcon(64, 64, icon.getPixelsPtr());  // }


    Map map(48.0f, level); // Zadeklarowanie mapy, bloki o wielkosci 48
    Player player(75, 75); // Zadeklarowanie gracza, ustawienie startowej pozycji na 50,50
    RayRender rayrender; // Zadeklarowanie rzutowania cieni

    while(window.isOpen()){ // Gra
        sf::Event event;

        float gameTime=gameClock.restart().asSeconds(); // Zegar gry


        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed) // Zamkniecie gry przy zamknieciu okna
                window.close();
            if((event.type==sf::Event::KeyPressed) && (event.key.code==sf::Keyboard::Escape)) // Zamkniecie gry przy wcisnieciu Escape
                window.close();
        }

        player.update(gameTime, map); // Aktualizacja pozycji gracza

//        player.update(gameTime);

        window.clear(); // Wyczyszczenie poprzedniej zawartosci okna

//        map.draw(window);
//        rayrender.drawRays(window, player, map);
//        player.draw(window);

        rayrender.render3D(window, player, map); // Rysowanie w perspektywie 3D

        window.display(); // Wyswietlenie obrazu
    }

    return 0;
}
