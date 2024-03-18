#include <SFML/Graphics.hpp>
#include <cmath>

const int windowWidth=800; // Szerokosc okna programu
const int windowHeight=600; // Wysokosc okna programu
const int mapWidth=10; // Szerokosc mapy
const int mapHeight=10; // Wysokosc mapy
const int blockSize=50; // Rozmiar bloku/sciany
const float playerSize=5.0f; // Rozmiar gracza

int map[mapWidth][mapHeight]={ // Definicja mapy
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

class Player{ // Definicja obiektu gracza
public:
    float x;
    float y;

    Player(float startX, float startY) : x(startX), y(startY) {}

    void move(float offsetX, float offsetY) {
        x+=offsetX;
        y+=offsetY;
    }
};

bool isColliding(float x, float y){ // Sprawdzanie kolizji
    int tileX=static_cast<int>(x/blockSize);
    int tileY=static_cast<int>(y/blockSize);
    return (tileX<0 || tileX>=mapWidth || tileY<0 || tileY>=mapHeight || map[tileX][tileY]==1);
}

int main(){
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Render Engine Project"); // Definicja okna
    Player player(75, 75); // Pozycja spawnu gracza (srodek drugiego bloku)

    while (window.isOpen()){ // Gra
        sf::Event event;
        while (window.pollEvent(event)) { // Zamkniecie gry przy zamknieciu okna
            if (event.type==sf::Event::Closed)
                window.close();
        }

        float moveX=0, moveY=0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            moveY-=0.5; // Ruch w gore
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            moveY+=0.5; // Ruch w dol
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            moveX-=0.5; // Ruch w lewo
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            moveX+=0.5; // Ruch w prawo
        }

        // Normalizacja ruchu (dlugosc wektora na 1)
        float length=std::sqrt(moveX*moveX+moveY*moveY);
        if (length!=0) {
            moveX/=length;
            moveY/=length;
        }

        // Sprawdzanie kolizji po osi X
        float newX = player.x + moveX;
        if (!isColliding(newX-playerSize, player.y-playerSize) &&
            !isColliding(newX+playerSize, player.y-playerSize) &&
            !isColliding(newX-playerSize, player.y+playerSize) &&
            !isColliding(newX+playerSize, player.y+playerSize)){
            player.x = newX;
        }

        // Sprawdzanie kolizji po osi Y
        float newY = player.y + moveY;
        if (!isColliding(player.x-playerSize, newY-playerSize) &&
            !isColliding(player.x+playerSize, newY-playerSize) &&
            !isColliding(player.x-playerSize, newY+playerSize) &&
            !isColliding(player.x+playerSize, newY+playerSize)){
            player.y = newY;
        }

        window.clear();

        // Rysowanie scian
        for (int i=0; i<mapWidth; ++i) {
            for (int j=0; j<mapHeight; ++j) {
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(blockSize, blockSize));
                rect.setPosition(i*blockSize, j*blockSize);
                rect.setFillColor(map[i][j]== 1 ? sf::Color::White : sf::Color::Black);
                window.draw(rect);
            }
        }

        // Rysowanie gracza jako blekitny okrag
        sf::CircleShape playerShape(playerSize);
        playerShape.setFillColor(sf::Color::Cyan);
        playerShape.setPosition(player.x-playerSize, player.y-playerSize);
        window.draw(playerShape);

        window.display();
    }

    return 0;
}
