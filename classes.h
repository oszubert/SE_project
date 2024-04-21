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

const float windowWidth=1152.0f; // Szerokosc okna programu
const float windowHeight=864.0f; // Wysokosc okna programu

const float playerFOV=65.0f; // Pole widzenia gracza
const unsigned int maxRayDepth=64; // Maksymalna glebokosc promienia
const unsigned int rayCount=600; // Zmienna do pomocnicza do rzutowania
const float columnWidth=windowWidth/(float)rayCount; // Zmienna pomocnicza do rzutowania

const float playerSize=8.0f; // Rozmiar gracza
const float moveSpeed=115.0f; // Predkosc ruchu gracza
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


    void update(float gameTime){ // Aktualizacja pozycji gracza

        float rad=angle*M_PI/180.0f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) angle-=rotSpeed*gameTime;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) angle+=rotSpeed*gameTime;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pos.x+=cos(rad)*moveSpeed*gameTime;
            pos.y+=sin(rad)*moveSpeed*gameTime;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos.x-=cos(rad)*moveSpeed*gameTime;
            pos.y-=sin(rad)*moveSpeed*gameTime;
        }
    }

    sf::Vector2f pos;
    float angle;
    sf::Texture tex;
};



struct Ray{ // Struktura promieni
    sf::Vector2f drawPos;
    sf::Vector2u mapPos;
    float dist;
    bool drawn;
    bool drawnVertical;
};

static Ray rayCast(sf::Vector2f start, float angleDeg, const Map &map){ // Rzucanie promieni
	float angle=angleDeg*M_PI/180.0f;
	float vert_tan=-tan(angle), hor_tan=-1.0f/tan(angle);
	float blockSize=map.getBlockSize();

	bool drawn=false;
	unsigned int vert_dof=0, hor_dof=0;
	float vert_dist=numeric_limits<float>::max();
	float hor_dist=numeric_limits<float>::max();
//	float vert_dist=FLOAT_MAX;
//	float hor_dist=FLOAT_MAX;

    sf::Vector2u vert_mapPos, hor_mapPos;
	sf::Vector2f vert_rayPos, hor_rayPos, offset;
	if (cos(angle)>0.001f){
		vert_rayPos.x=floor(start.x/blockSize)*blockSize+blockSize;
		vert_rayPos.y=(start.x-vert_rayPos.x)*vert_tan+start.y;

		offset.x=blockSize;
		offset.y=-offset.x*vert_tan;
	}
	else if (cos(angle)<-0.001f){
		vert_rayPos.x=floor(start.x/blockSize)*blockSize-0.01f;
		vert_rayPos.y=(start.x-vert_rayPos.x)*vert_tan+start.y;

		offset.x=-blockSize;
		offset.y=-offset.x*vert_tan;
	}
	else vert_dof=maxRayDepth;

	const auto &level=map.getLevel();

	for (; vert_dof<maxRayDepth; vert_dof++){
		int mapX=(int)(vert_rayPos.x/blockSize);
		int mapY=(int)(vert_rayPos.y/blockSize);

		if (mapY<level.size() && mapX<level[mapY].size() && level[mapY][mapX]!=sf::Color::Black){
			drawn=true;
			vert_dist=sqrt(
                (vert_rayPos.x-start.x)*(vert_rayPos.x-start.x) +
                (vert_rayPos.y-start.y)*(vert_rayPos.y-start.y)
            );
			vert_mapPos=sf::Vector2u(mapX, mapY);
			break;
		}

		vert_rayPos+=offset;
	}

	if(sin(angle)>0.001f){
		hor_rayPos.y=floor(start.y/blockSize)*blockSize+blockSize;
		hor_rayPos.x=(start.y-hor_rayPos.y)*hor_tan+start.x;

		offset.y=blockSize;
		offset.x=-offset.y*hor_tan;
	}
	else if (sin(angle)<-0.001f){
		hor_rayPos.y=floor(start.y/blockSize)*blockSize-0.01f;
		hor_rayPos.x=(start.y-hor_rayPos.y)*hor_tan+start.x;

		offset.y=-blockSize;
		offset.x=-offset.y*hor_tan;
	}
	else hor_dof=maxRayDepth;

	for (; hor_dof<maxRayDepth; hor_dof++){
		int mapX=(int)(hor_rayPos.x/blockSize);
		int mapY=(int)(hor_rayPos.y/blockSize);

		if (mapY<level.size() && mapX<level[mapY].size() && level[mapY][mapX]!=sf::Color::Black){
			drawn=true;
			hor_dist=sqrt(
                (hor_rayPos.x-start.x)*(hor_rayPos.x-start.x)+
                (hor_rayPos.y-start.y)*(hor_rayPos.y-start.y)
            );
			hor_mapPos=sf::Vector2u(mapX, mapY);
			break;
		}

		hor_rayPos+=offset;
	}

	return Ray{hor_dist<vert_dist ? hor_rayPos : vert_rayPos, hor_dist < vert_dist ? hor_mapPos : vert_mapPos, min(hor_dist, vert_dist), drawn, vert_dist <= hor_dist};
}

class RayRender{ // Rysowanie promieni
public:
    void drawRays(sf::RenderTarget &target, const Player &player, const Map &map){ // Rysowanie promieni
        for(float angle=player.angle-playerFOV/2.0f; angle<player.angle+playerFOV; angle+=0.5f){
            Ray ray=rayCast(player.pos, angle, map);

            if(ray.drawn){
                sf::Vertex line[]={sf::Vertex(player.pos), sf::Vertex(ray.drawPos),};
                line[0].color=sf::Color::Cyan;
                line[1].color=sf::Color::Cyan;
                target.draw(line, 2, sf::Lines);
            }
        }
    }

    void render3D(sf::RenderTarget &target, const Player &player, const Map &map){ // Rysowanie w 3D (tymczasowe, zostanie zastapione algorytmem Digital Differential Analysis
        sf::RectangleShape rectangle(sf::Vector2f(windowWidth, windowHeight/2.0f));
        rectangle.setFillColor(sf::Color(81, 187, 254)); // Kolor skyboxa
        target.draw(rectangle);
        const sf::Color fogColor=sf::Color(81, 187, 254);

        rectangle.setPosition(0.0f, windowHeight / 2.0f);
        rectangle.setFillColor(sf::Color(14, 176, 92)); // Kolor ziemii
        target.draw(rectangle);


        float angle=player.angle-playerFOV/2.0f;
        float angleMove=playerFOV/(float)rayCount;
        const float maxRenderDist=maxRayDepth*map.getBlockSize();
        const float maxFogDist=maxRenderDist/4.0f;
        for(unsigned int i=0; i<rayCount; i++, angle+=angleMove){
            Ray ray=rayCast(player.pos, angle, map);

            if(ray.drawn){
                ray.dist*=cos((player.angle-angle)*M_PI/180.0f);

                float wallHeight=(map.getBlockSize()*windowHeight)/ray.dist;

                if(wallHeight>windowHeight)wallHeight=windowHeight;

                float brightness=1.0f-(ray.dist/maxRenderDist);
                if(brightness<0.0f) brightness=0.0f;

                float shade=(ray.drawnVertical ? 0.8f : 1.0f)*brightness;

                float wallOffset=windowHeight/2.0f-wallHeight/2.0f;
                sf::RectangleShape column(sf::Vector2f(columnWidth, wallHeight));
                column.setPosition(i*columnWidth, wallOffset);

                float fogVisibility=(ray.dist/maxFogDist);
                if(fogVisibility>1.0f) fogVisibility=1.0f;

                sf::Color color=map.getLevel()[ray.mapPos.y][ray.mapPos.x];
                color=sf::Color(color.r*shade, color.g*shade, color.b*shade);
                column.setFillColor(sf::Color(
                    (1.0f-fogVisibility)*color.r+fogVisibility*fogColor.r,
                    (1.0f-fogVisibility)*color.g+fogVisibility*fogColor.g,
                    (1.0f-fogVisibility)*color.b+fogVisibility*fogColor.b)
                );

                target.draw(column);
            }
        }
    }
};


#endif // klasy
