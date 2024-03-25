#ifndef klasy
#define klasy

//#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <math.h>
#include <cmath>
#include <vector>

#include <windows.h>

using namespace std;

const float M_PI = 3.14159265358979323846;

const int windowWidth=800; // Szerokosc okna programu
const int windowHeight=600; // Wysokosc okna programu
const int mapWidth=10; // Szerokosc mapy
const int mapHeight=10; // Wysokosc mapy
const int blockSize=50; // Rozmiar bloku/sciany

const float playerSize=8.0f; // Rozmiar gracza
const float moveSpeed=50.0f; // Predkosc ruchu gracza
const float rotSpeed=100.0f; // Predkosc obrotu gracza

class Map{ // Definicja klasy mapy
private:
    vector<vector<int>> level;
    float blockSize;
public:
    Map(float blockSize, int width, int height) : blockSize(blockSize), level(height, vector<int>(width, 0)) {}
    Map(float blockSize, vector<vector<int>> level) : blockSize(blockSize), level(level) {}

    void draw(sf::RenderTarget &target){
        if (level.empty()) return;

        sf::RectangleShape background(sf::Vector2f((float)level[0].size()*blockSize, (float)level.size()*blockSize));
        background.setFillColor(sf::Color(15,15,15));
        target.draw(background);

        sf::RectangleShape block(sf::Vector2f(blockSize * 0.95f, blockSize * 0.95f));

        for(unsigned int y=0; y<level.size(); y++){
            for(unsigned int x=0; x<level[y].size(); x++){
                if(level[y][x]==0) block.setFillColor(sf::Color(11, 32, 39));
                else if(level[y][x]==1) block.setFillColor(sf::Color(64, 121, 140));

                block.setPosition(sf::Vector2f(x, y)*blockSize+sf::Vector2f(blockSize*0.025f, blockSize*0.025f));
                target.draw(block);
            }
        }
    }

    float getBlockSize() const { return blockSize; }
    const std::vector<std::vector<int>>& getLevel() const { return level; }

};

class Player{ // Definicja klasy gracza
public:
    Player(float x, float y) : pos(sf::Vector2f(x,y)) {}

    void draw(sf::RenderTarget &target){
        sf::CircleShape playerShape(playerSize);
        playerShape.setOrigin(playerShape.getRadius(), playerShape.getRadius());
        playerShape.setPosition(pos);
        playerShape.setFillColor(sf::Color::Cyan);
        playerShape.setRotation(angle);

        tex.loadFromFile("ball.png");
        playerShape.setTexture(&tex);
        playerShape.setTextureRect(sf::IntRect(0, 0, 16, 16));

/*
        sf::RectangleShape line(sf::Vector2f(24.0f, 2.0f));
        line.setPosition(pos);
        line.setRotation(angle);
        line.setFillColor(sf::Color::Blue);

        target.draw(line);*/
        target.draw(playerShape);
	}


    void update(float gameTime, const Map& map){

       /* float rad=angle*M_PI/180.0f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) angle-=rotSpeed*gameTime;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) angle+=rotSpeed*gameTime;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pos.x+=cos(rad)*moveSpeed*gameTime;
            pos.y+=sin(rad)*moveSpeed*gameTime;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos.x-=cos(rad)*moveSpeed*gameTime;
            pos.y-=sin(rad)*moveSpeed*gameTime;
        }*/


        // Tymczasowe rozwiazanie kolizji

        float rad=angle*M_PI/180.0f;

        sf::Vector2f newPos=pos;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) angle-=rotSpeed*gameTime;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) angle+=rotSpeed*gameTime;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            newPos.x+=cos(rad)*moveSpeed*gameTime;
            newPos.y+=sin(rad)*moveSpeed*gameTime;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            newPos.x-=cos(rad)*moveSpeed*gameTime;
            newPos.y-=sin(rad)*moveSpeed*gameTime;
        }

        if(!checkCollision(newPos, map)){
            pos=newPos;
        }
    }

    bool checkCollision(const sf::Vector2f& newPos, const Map& map) const{
        int gridX=static_cast<int>((newPos.x+playerSize)/map.getBlockSize());
        int gridY=static_cast<int>((newPos.y+playerSize)/map.getBlockSize());

        int leftGridX=static_cast<int>((newPos.x - playerSize)/map.getBlockSize());
        int topGridY=static_cast<int>((newPos.y - playerSize)/map.getBlockSize());

        if (gridX<0 || gridX>=map.getLevel()[0].size() || gridY<0 || gridY>=map.getLevel().size() ||
            leftGridX<0 || leftGridX>=map.getLevel()[0].size() || topGridY<0 || topGridY>=map.getLevel().size()) {
            return true;
        }

        if(map.getLevel()[gridY][gridX]==1 || map.getLevel()[gridY][leftGridX]==1 ||
            map.getLevel()[topGridY][gridX]==1 || map.getLevel()[topGridY][leftGridX]==1){
            return true;
        }

        return false;
    }


    sf::Vector2f pos;
    float angle;
    sf::Texture tex;
};



#endif // klasy
