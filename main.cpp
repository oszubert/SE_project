#include "classes.h"

sf::Clock gameClock; // Zegar gry (aby gra dzialala niezaleznie od ilosci klatek)

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) { // Funkcja main bez uruchamiania konsoli
//    AllocConsole(); freopen("CONOUT$", "w+", stdout); // konsola dla debugowania


    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Render Engine Project"); // Definicja okna

//    window.setVerticalSyncEnabled(true); // vSync

    window.setFramerateLimit(60); // Limit klatek ustawiony na 60

//  srednia fps
    float avgFps = 1.0;

    sf::Image icon; // } Ikona programu
    icon.loadFromFile("icon.png"); // }
    window.setIcon(64, 64, icon.getPixelsPtr());  // }


    Map map(48.0f, "arena.png"); // Zadeklarowanie mapy, bloki o wielkosci 48, ladowanie z pliku arena.png
    Player player; // Zadeklarowanie gracza
    player.pos=sf::Vector2f(100, 100); // Ustawienie pozycji gracza na (100,100)
    RayRender rayrender; // Zadeklarowanie rzutowania cieni
    rayrender.init();

    Editor editor{}; // Deklaracja edytora
    editor.init(window); // Inicjalizacja edytora

    enum class State {Editor, Game} gamestate=State::Game; // Stan gry (czy gracz jest w edytorze, czy w grze)

    sf::Mouse mouse;
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    while(window.isOpen()){ // Gra
        sf::Event event;

        float gameTime=gameClock.restart().asSeconds(); // Zegar gry

        // Wyswietla klatki na sekunde w tytule okna
        avgFps = 0.9 * avgFps + 0.1 * (1/gameTime);
        window.setTitle("Render Engine Project      ||      Fps: " + to_string(static_cast<int>(avgFps)));

        while(window.pollEvent(event)){
            if(event.type==sf::Event::Closed) // Zamkniecie gry przy zamknieciu okna
                window.close();
            else if((event.type==sf::Event::KeyPressed) && (event.key.code==sf::Keyboard::Escape)) // Zamkniecie gry przy wcisnieciu Escape
                window.close();
            else if((event.type==sf::Event::KeyPressed) && (event.key.code==sf::Keyboard::Tab))
                gamestate = gamestate == State::Game ? State::Editor : State::Game;

            if (gamestate==State::Editor) editor.handleEvent(event);
        }

        window.clear(); // Wyczyszczenie poprzedniej zawartosci okna

        if(gamestate==State::Game){
            window.setView(window.getDefaultView());
            rayrender.render3D(window, player, map); // Rysowanie w perspektywie 3D
            player.update(gameTime, mouse.getPosition(window)); // Aktualizacja pozycji gracza
            mouse.setPosition(sf::Vector2i(window.getSize())/2, window); //Resetowanie myszy do srodka ekranu
//       ^ nieplynny ruch, potencjalna przyszla zmiana na raw input!!

        }
        else{
            editor.run(window, map);
            map.draw(window);
        }


        window.display(); // Wyswietlenie obrazu
    }

    return 0;
}
