#include "classes.h"


sf::Clock gameClock; // Zegar gry (aby gra dzialala niezaleznie od ilosci klatek)

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) { // Funkcja main bez uruchamiania konsoli
    //AllocConsole(); freopen("CONOUT$", "w+", stdout); // konsola dla debugowania


    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Render Engine Project"); // Definicja okna

    window.setFramerateLimit(60); // Limit klatek ustawiony na 60


    sf::Image icon; // } Ikona programu
    icon.loadFromFile("icon.png"); // }
    window.setIcon(64, 64, icon.getPixelsPtr());  // }


    Map map(48.0f, "arena.png"); // Zadeklarowanie mapy, bloki o wielkosci 48, ladowanie z pliku arena.png
    Player player; // Zadeklarowanie gracza
    player.pos=sf::Vector2f(100, 100); // Ustawienie pozycji gracza na (100,100)
    RayRender rayrender; // Zadeklarowanie rzutowania cieni
    rayrender.init();

    while(window.isOpen()){ // Gra
        sf::Event event;


        float gameTime=gameClock.restart().asSeconds(); // Zegar gry

        window.setMouseCursorGrabbed(true);
        window.setMouseCursorVisible(false);

        sf::Mouse mouse;
        cout<<mouse.getPosition(window).x<<endl;
        int t = 8;
//        mouse.setPosition(mouse.getPosition(window) + (sf::Vector2i(window.getSize()/2) - mouse.getPosition(window))*t/10, window);
        mouse.setPosition( (mouse.getPosition(window) + sf::Vector2i(window.getSize())/2)/ 2 , window);

        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed) // Zamkniecie gry przy zamknieciu okna
                window.close();
            if((event.type==sf::Event::KeyPressed) && (event.key.code==sf::Keyboard::Escape)) // Zamkniecie gry przy wcisnieciu Escape
                window.close();
        }

        player.update(gameTime, mouse.getPosition(window).x); // Aktualizacja pozycji gracza

        window.clear(); // Wyczyszczenie poprzedniej zawartosci okna

//        map.draw(window);
//        rayrender.drawRays(window, player, map);
//        player.draw(window);

        rayrender.render3D(window, player, map); // Rysowanie w perspektywie 3D

        window.display(); // Wyswietlenie obrazu
    }

    return 0;
}
