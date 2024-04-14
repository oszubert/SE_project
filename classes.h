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

//const float M_PI=3.14159265358979323846;

const float windowWidth=1152.0f; // Szerokosc okna programu
const float windowHeight=864.0f; // Wysokosc okna programu
//const int mapWidth=10; // Szerokosc mapy
//const int mapHeight=10; // Wysokosc mapy
//const int blockSize=50; // Rozmiar bloku/sciany

const float playerFOV=65.0f; // Pole widzenia gracza
const unsigned int maxRayDepth=16; // Maksymalna glebokosc promienia
const unsigned int rayCount=600; // Zmienna do pomocnicza do rzutowania
const float columnWidth=windowWidth/(float)rayCount; // Zmienna pomocnicza do rzutowania

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
        background.setFillColor(sf::Color(15,15,15)); // Kolor tla (rzutowanie 2D)
        target.draw(background);

        sf::RectangleShape block(sf::Vector2f(blockSize * 0.95f, blockSize * 0.95f));

        for(unsigned int y=0; y<level.size(); y++){
            for(unsigned int x=0; x<level[y].size(); x++){
                if(level[y][x]==0) block.setFillColor(sf::Color(11, 32, 39)); // Kolor podloza (rzutowanie 2D)
                else if(level[y][x]==1) block.setFillColor(sf::Color(64, 121, 140)); // Kolor bloku (sciana, rzutowanie 2D)

                block.setPosition(sf::Vector2f(x, y)*blockSize+sf::Vector2f(blockSize*0.025f, blockSize*0.025f));
                target.draw(block);
            }
        }
    }

    float getBlockSize() const {return blockSize;}
    const vector<vector<int>>& getLevel() const {return level;}
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
        // Nieuzywana linia wskazujaca obrot postaci (zastapiona przez obracajaca sie teksture postaci)
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



struct Ray{
    sf::Vector2f drawPos;
    float dist;
    bool drawn;
    bool drawnVertical;
};

static Ray rayCast(sf::Vector2f start, float angleDeg, const Map &map){ // Rzucanie promieni
	float angle=angleDeg*M_PI/180.0f;
	float vert_tan=-tan(angle), hor_tan=-1.0f/tan(angle);
	float blockSize=map.getBlockSize();

	bool drawn=false;
	size_t vert_dof=0, hor_dof=0;
	float vert_dist=numeric_limits<float>::max();
	float hor_dist=numeric_limits<float>::max();
//	float vert_dist=FLOAT_MAX;
//	float hor_dist=FLOAT_MAX;

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
	else{
		vert_dof=maxRayDepth;
	}

	const auto &level=map.getLevel();

	for (; vert_dof<maxRayDepth; vert_dof++){
		int mapX=(int)(vert_rayPos.x/blockSize);
		int mapY=(int)(vert_rayPos.y/blockSize);

		if (mapY<level.size() && mapX<level[mapY].size() && level[mapY][mapX]){
			drawn=true;
			vert_dist=sqrt((vert_rayPos.x-start.x)*(vert_rayPos.x-start.x)+(vert_rayPos.y-start.y)*(vert_rayPos.y-start.y));
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
	else{
		hor_dof=maxRayDepth;
	}

	for (; hor_dof<maxRayDepth; hor_dof++){
		int mapX=(int)(hor_rayPos.x/blockSize);
		int mapY=(int)(hor_rayPos.y/blockSize);

		if (mapY<level.size() && mapX<level[mapY].size() && level[mapY][mapX]){
			drawn=true;
			hor_dist=sqrt((hor_rayPos.x-start.x)*(hor_rayPos.x-start.x)+(hor_rayPos.y-start.y)*(hor_rayPos.y-start.y));
			break;
		}

		hor_rayPos+=offset;
	}

	return Ray{hor_dist<vert_dist ? hor_rayPos : vert_rayPos, min(hor_dist, vert_dist), drawn, vert_dist <= hor_dist};

}

class RayRender{
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

    void render3D(sf::RenderTarget &target, const Player &player, const Map &map){ // Rysowanie w 3D

        sf::RectangleShape rectangle(sf::Vector2f(windowWidth, windowHeight/2.0f));
        rectangle.setFillColor(sf::Color(81, 187, 254)); // Kolor skyboxa
        target.draw(rectangle);

        rectangle.setPosition(0.0f, windowHeight / 2.0f);
        rectangle.setFillColor(sf::Color(14, 176, 92)); // Kolor ziemii
        target.draw(rectangle);

        float angle=player.angle-playerFOV/2.0f;
        float angleMove=playerFOV/(float)rayCount;
        const float maxRenderDist=maxRayDepth*map.getBlockSize();
        const float maxFogDist=maxRenderDist/4.0f;
        for (unsigned int i=0; i<rayCount; i++, angle+=angleMove){
            Ray ray=rayCast(player.pos, angle, map);

            if (ray.drawn){
                ray.dist*=cos((player.angle-angle)*M_PI/180.0f);

                float wallHeight=(map.getBlockSize()*windowHeight)/ray.dist;

                if (wallHeight>windowHeight)wallHeight=windowHeight;

                float brightness=1.0f-(ray.dist/maxRenderDist);
                if (brightness<0.0f) brightness=0.0f;

                float shade=(ray.drawnVertical ? 0.8f : 1.0f)*brightness;

                float wallOffset=windowHeight/2.0f-wallHeight/2.0f;
                sf::RectangleShape column(sf::Vector2f(columnWidth, wallHeight));
                column.setPosition(i*columnWidth, wallOffset);

                column.setFillColor(sf::Color(199*shade, 0*shade, 57*shade)); // Kolor scian
                target.draw(column);
            }
        }
    }
};


#endif // klasy
