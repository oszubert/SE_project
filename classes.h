#ifndef klasy
#define klasy

//#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

#include <windows.h>

using namespace std;

//const float M_PI=3.14159265358979323846;

//const float windowWidth=1152.0f; // Szerokosc okna programu
//const float windowHeight=864.0f; // Wysokosc okna programu
const float windowWidth=800.0f; // Szerokosc okna programu
const float windowHeight=600.0f; // Wysokosc okna programu

const float playerFOV=65.0f; // Pole widzenia gracza
const float cameraZ=0.5f*windowHeight; // Wysokosc kamery
const unsigned int maxRayDepth=64; // Maksymalna glebokosc promienia
//const unsigned int rayCount=600; // Zmienna do pomocnicza do rzutowania
//const float columnWidth=windowWidth/(float)rayCount; // Zmienna pomocnicza do rzutowania

const float playerSize=8.0f; // Rozmiar gracza
const float moveSpeed=115.0f; // Predkosc ruchu gracza
const float sprintMultiplier=1.8f; // Mnoznik biegania
const float rotSpeed=115.0f; // Predkosc obrotu gracza


class Map{ // Definicja klasy mapy
private:
    vector<vector<sf::Color>> level;
    float blockSize;
public:
    Map(float blockSize, int width, int height) : blockSize(blockSize), level(height, vector<sf::Color>(width, sf::Color::Black)) {}
    Map(float blockSize, const string &mapfile): blockSize(blockSize){
        sf::Image source;
        if(!source.loadFromFile(mapfile)){
            cerr << "Nie mozna zaladowac pliku z mapa." << endl;
            return;
        }

        level=vector<vector<sf::Color>>(source.getSize().y, vector<sf::Color>(source.getSize().x, sf::Color::Black));


        for(unsigned int y=0; y<source.getSize().y; y++){
            for(unsigned int x=0; x<source.getSize().x; x++) level[y][x]=source.getPixel(x,y);
        }

    }

    void draw(sf::RenderTarget &target){ //Rysowanie mapy (2D, niewymagane w 3D)
        if (level.empty()) return;

        sf::RectangleShape background(sf::Vector2f((float)level[0].size()*blockSize, (float)level.size()*blockSize));
        background.setFillColor(sf::Color(15,15,15)); // Kolor tla (rzutowanie 2D)
        target.draw(background);

        sf::RectangleShape block(sf::Vector2f(blockSize * 0.95f, blockSize * 0.95f));

        for(unsigned int y=0; y<level.size(); y++){
            for(unsigned int x=0; x<level[y].size(); x++){
                block.setFillColor(level[y][x]);

                block.setPosition(sf::Vector2f(x, y)*blockSize+sf::Vector2f(blockSize*0.025f, blockSize*0.025f));
                target.draw(block);
            }
        }
    }

    float getBlockSize() const {return blockSize;}
    const vector<vector<sf::Color>>& getLevel() const {return level;}
};



class Player{ // Definicja klasy gracza
public:
//    Player(float x, float y) : pos(sf::Vector2f(x,y)) {}

    void draw(sf::RenderTarget &target){ // Rysowanie postaci w 2D (niewymagane w 3D)
        sf::CircleShape playerShape(playerSize);
        playerShape.setOrigin(playerShape.getRadius(), playerShape.getRadius());
        playerShape.setPosition(pos);
        playerShape.setFillColor(sf::Color::Cyan);
        playerShape.setRotation(angle);

        tex.loadFromFile("ball.png");
        playerShape.setTexture(&tex);
        playerShape.setTextureRect(sf::IntRect(0, 0, 16, 16));

        target.draw(playerShape);
	}


    void update(float gameTime, sf::Vector2i mousePos){ // Aktualizacja pozycji gracza

        float rad=angle*M_PI/180.0f;

        float mouseSens = 12.5;
        //angle += mouse.getPosition().x * gameTime * mouseSens;
        angle += (mousePos.x - windowWidth/2) * mouseSens * gameTime;


//        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) angle-=rotSpeed*gameTime;
//        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) angle+=rotSpeed*gameTime;
        float currentMoveSpeed;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            currentMoveSpeed = moveSpeed * sprintMultiplier;
        else currentMoveSpeed = moveSpeed;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pos.x+=cos(rad)*currentMoveSpeed*gameTime;
            pos.y+=sin(rad)*currentMoveSpeed*gameTime;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos.x-=cos(rad)*currentMoveSpeed*gameTime;
            pos.y-=sin(rad)*currentMoveSpeed*gameTime;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            pos.x+=sin(rad)*currentMoveSpeed*gameTime;
            pos.y-=cos(rad)*currentMoveSpeed*gameTime;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            pos.x-=sin(rad)*currentMoveSpeed*gameTime;
            pos.y+=cos(rad)*currentMoveSpeed*gameTime;
        }
    }

    sf::Vector2f pos;
    float angle;
    sf::Texture tex;
};


class RayRender{ // Rysowanie promieni
private:
    sf::Texture wallTex;
    sf::Image floorImage;
public:
    void init(){ // Tworzenie tekstury i sprite
        if (!wallTex.loadFromFile("brick.png")){
            cerr<<"Nie udalo sie zaladowac brick.png!";}
        if(wallTex.getSize().x != wallTex.getSize().y){
            cerr<<"Tekstura brick.png nie jest kwadratem!";}

        if (!floorImage.loadFromFile("floor.png")){
            cerr<<"Nie udalo sie zaladowac floor.png!";}
        if(floorImage.getSize().x != floorImage.getSize().y){
            cerr<<"Tekstura floor.png nie jest kwadratem!";}
    }

    void render3D(sf::RenderTarget &target, const Player &player, const Map &map){ // Rysowanie w 3D (tymczasowe, zostanie zastapione algorytmem Digital Differential Analysis
        sf::RectangleShape rectangle(sf::Vector2f(windowWidth, windowHeight/2.0f));
        rectangle.setFillColor(sf::Color(81, 187, 254)); // Kolor skyboxa
        target.draw(rectangle);

//        rectangle.setPosition(0.0f, windowHeight / 2.0f);
//        rectangle.setFillColor(sf::Color(14, 176, 92)); // Kolor ziemii
//        target.draw(rectangle);

        float rad=player.angle*M_PI/180.0f;
        sf::Vector2f direction{cos(rad), sin(rad)};
        sf::Vector2f plane{-direction.y, direction.x * 0.66f};
        sf::Vector2f position=player.pos/map.getBlockSize();

        //Problem z za duzym rozmiarem listy!!
        uint8_t floorPixels[(size_t)windowWidth * (size_t)windowHeight * (size_t)4]{};
        for(size_t y=windowHeight/2; y<windowHeight; y++){
            sf::Vector2f rayDirLeft{direction-plane}, rayDirRight{direction+plane};
            float rowDistance = cameraZ / ((float)y-windowHeight/2);

            sf::Vector2f floorStep = rowDistance*(rayDirRight-rayDirLeft)/windowWidth;
            sf::Vector2f floorPos = position+rowDistance*rayDirLeft;

            for(size_t x=0; x<windowWidth; x++){
                sf::Vector2i cell{floorPos};

                float texSize = floorImage.getSize().x;
                sf::Vector2i texCoords{texSize*(floorPos-(sf::Vector2f)cell)};
                texCoords.x &= (int)texSize-1;
                texCoords.y &= (int)texSize-1;
//                floorPixels.append(sf::Vertex(sf::Vector2f(x,y), texCoords));
                sf::Color color = floorImage.getPixel(texCoords.x, texCoords.y);
                floorPixels[(x+y*(size_t)windowWidth)*4 + 0] = color.r;
                floorPixels[(x+y*(size_t)windowWidth)*4 + 1] = color.g;
                floorPixels[(x+y*(size_t)windowWidth)*4 + 2] = color.b;
                floorPixels[(x+y*(size_t)windowWidth)*4 + 3] = color.a;

                floorPos += floorStep;
            }
        }

        sf::Image image;
        image.create((unsigned int)windowWidth, (unsigned int)windowHeight, floorPixels);
        sf::Texture texture;
        texture.loadFromImage(image);
        sf::Sprite sprite{texture};
        target.draw(sprite);

        sf::VertexArray walls{sf::Lines};
        for(unsigned int i=0; i<windowWidth; i++){
            float cameraX=i*2.0f/windowWidth-1.0f;
            sf::Vector2f rayPos=position;
            sf::Vector2f rayDir=direction+plane*cameraX;

            sf::Vector2f deltaDist{
                abs(1.0f/rayDir.x),
                abs(1.0f/rayDir.y),
            };

            sf::Vector2i mapPos{rayPos};
            sf::Vector2i step;
            sf::Vector2f sideDist;

            if (rayDir.x<0.0f){
                step.x=-1;
                sideDist.x=(-mapPos.x+rayPos.x)*deltaDist.x;
            }
            else{
                step.x=1;
                sideDist.x=(mapPos.x-rayPos.x+1.0f)*deltaDist.x;
            }

            if(rayDir.y<0.0f){
                step.y=-1;
                sideDist.y=(-mapPos.y+rayPos.y)*deltaDist.y;
            }
            else{
                step.y=1;
                sideDist.y=(mapPos.y-rayPos.y+1.0f)*deltaDist.y;
            }
            bool rayHit{}, hitVertical{};

            unsigned int depth=0;
            while(!rayHit && depth<maxRayDepth){
                if(sideDist.x<sideDist.y){
                    sideDist.x+=deltaDist.x;
                    mapPos.x+=step.x;
                    hitVertical=false;
                }
                else{
                    sideDist.y+=deltaDist.y;
                    mapPos.y+=step.y;
                    hitVertical=true;
                }

                int x=mapPos.x, y=mapPos.y;
                const auto &level=map.getLevel();
                if (y>=0 && y<level.size() && x>=0 && x<level[y].size() && level[y][x]!=sf::Color::Black) rayHit=true;
                depth++;
            }

            if(rayHit){
                float perpWallDist=hitVertical ? sideDist.y-deltaDist.y : sideDist.x-deltaDist.x;
                float wallHeight=windowHeight/perpWallDist;
                float wallStart=(-wallHeight+windowHeight)/2.0f;
                float wallEnd=(wallHeight+windowHeight)/2.0f;
                float textureSize=wallTex.getSize().x;

                float wallX=hitVertical ? rayPos.x+perpWallDist*rayDir.x : rayPos.y+perpWallDist*rayDir.y;
                wallX-=floor(wallX);
                float textureX=wallX*textureSize;

                float brightness=1.0f-(perpWallDist/(float)maxRayDepth);
                if(hitVertical) brightness*=0.7f;

                sf::Color color=sf::Color(255*brightness, 255*brightness, 255*brightness);

                walls.append(sf::Vertex(sf::Vector2f((float)i, wallStart), color, sf::Vector2f(textureX, 0.0f)));
                walls.append(sf::Vertex(sf::Vector2f((float)i, wallEnd), color, sf::Vector2f(textureX, textureSize)));
            }

        }
        sf::RenderStates states{&wallTex};
        target.draw(walls, states);
    }
};


#endif // klasy
