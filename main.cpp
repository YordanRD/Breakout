#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <math.h>

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif


#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

using namespace sf;
using namespace std;

const float pi = 3.14159f;

RenderWindow window;

Font font;
Text gameoverText;
Text lifeText;
Text scoreText;

Clock gameClock;
float deltaTime;

float frameWidth = 800;
float frameHeight = 800;

bool isPlaying = false;
bool gameover = false;
bool win = false;

int life = 3;
int score = 0;
int combo = 0;

const float startposX = 55;
const float startposY = 70;

Paddle paddle;
Ball ball;

Texture textureBall;
RectangleShape background;
Texture textureBack;
Texture texturePaddle;
Texture textureBrick;



vector<Brick*> bricks;

void Initiate();
void Reset();
void Update();
void Render();
void HandleInput();
void loadLevel();

bool BallLeft(RectangleShape rect);
bool BallRight(RectangleShape rect);
bool BallUp(RectangleShape rect);
bool BallBottom(RectangleShape rect);

int main()
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);


	window.create(VideoMode(frameWidth, frameHeight), "Breakout");
	Initiate();
	loadLevel();
	while (window.isOpen())
	{

		deltaTime = gameClock.restart().asSeconds();
		HandleInput();


		if (isPlaying && !gameover && !win)
		{
			Update();
		}



		Render();
	}

	return EXIT_SUCCESS;


}

void Initiate()
{
	font.loadFromFile("consola.ttf");

	textureBall.loadFromFile("ball.png");
	textureBack.loadFromFile("back.png");
	texturePaddle.loadFromFile("paddle.png");
	textureBrick.loadFromFile("brick.png");

	background.setSize(Vector2f(frameWidth, frameHeight));
	background.setPosition(0, 0);
	background.setTexture(&textureBack);

	lifeText.setFont(font);
	lifeText.setCharacterSize(20);
	lifeText.setPosition(620, frameHeight - 30);
	lifeText.setString("life:" + std::to_string(life));

	gameoverText.setFont(font);
	gameoverText.setCharacterSize(35);
	gameoverText.setPosition(100, 400);
	gameoverText.setString("");

	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setPosition(80, frameHeight - 30);
	scoreText.setString("score:" + std::to_string(score));

}

void Reset()
{
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;

}

void Update()
{
	if (ball.angle < 0)
	{
		ball.angle += 2 * pi;
	}
	ball.angle = fmod(ball.angle, 2 * pi);

	float factor = ball.speed * deltaTime;
	ball.picture.move(std::cos(ball.angle) * factor, std::sin(ball.angle) * factor);
	//physics
	//edge
	if (ball.picture.getPosition().y + ball.picture.getRadius() > frameHeight)
	{
		isPlaying = false;
		life--;
		
		Reset();
	}
	else if (ball.picture.getPosition().x - ball.picture.getRadius() < 50.f)
	{
		ball.angle = pi - ball.angle;
		ball.picture.setPosition(ball.picture.getRadius() + 50.1f, ball.picture.getPosition().y);
		
	}
	else if (ball.picture.getPosition().x + ball.picture.getRadius() > frameWidth - 50)
	{
		ball.angle = pi - ball.angle;
		ball.setPosition(frameWidth - ball.picture.getRadius() - 50.1f, ball.picture.getPosition().y);
		
	}
	else if (ball.picture.getPosition().y - ball.picture.getRadius() < 50.f)
	{
		ball.angle = -ball.angle;
		ball.setPosition(ball.picture.getPosition().x, ball.picture.getRadius() + 50.1f);
		
	}

	//paddle
	if (BallBottom(paddle.picture))
	{
		int dis = ball.picture.getPosition().x - paddle.picture.getPosition().x;
		if (dis > 30 && ball.angle > 1.f / 2.f * pi)
		{
			ball.angle = ball.angle - pi;
		}
		else if (dis < -30 && ball.angle < 1.f / 2.f * pi)
		{
			ball.angle = ball.angle - pi;
		}
		else
		{
			ball.angle = -ball.angle;
			if (ball.angle > 1.f / 2.f * pi && ball.angle < 7.f / 8.f * pi)
			{
				ball.angle += (std::rand() % 15) * pi / 180;
			}
			else if (ball.angle < 1.f / 2.f * pi && ball.angle > 1.f / 8.f * pi)
			{
				ball.angle -= (std::rand() % 15) * pi / 180;
			}
			else if (ball.angle <= 1.f / 8.f * pi)
			{
				ball.angle += (std::rand() % 15) * pi / 180;
			}
			else if (ball.angle >= 7.f / 8.f * pi)
			{
				ball.angle -= (std::rand() % 15) * pi / 180;
			}
		}

		combo = 0;
		ball.setPosition(ball.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
		
	}
	//bricks
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			if (bricks[i]->speed != 0.f)
			{
				if (bricks[i]->picture.getPosition().x - bricks[i]->picture.getSize().x / 2 < 50.f)
					bricks[i]->moveLeft = false;
				else if (bricks[i]->picture.getPosition().x + bricks[i]->picture.getSize().x / 2 > frameWidth - 50.f)
					bricks[i]->moveLeft = true;

				if (bricks[i]->moveLeft)
					bricks[i]->picture.move(-bricks[i]->speed * deltaTime, 0.0f);
				else
					bricks[i]->picture.move(bricks[i]->speed * deltaTime, 0.0f);


			}


			if (BallUp(bricks[i]->picture))
			{
				ball.angle = -ball.angle;
				ball.setPosition(ball.picture.getPosition().x, bricks[i]->picture.getPosition().y + bricks[i]->picture.getSize().y / 2 + ball.picture.getRadius() + 0.1f);
				if (bricks[i]->hit())
				{
					
				}
				else
				{
					
				}
				combo++;
				score = score + combo * 10;
			}
			else if (BallBottom(bricks[i]->picture))
			{
				ball.angle = -ball.angle;
				ball.setPosition(ball.picture.getPosition().x, bricks[i]->picture.getPosition().y - bricks[i]->picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
				if (bricks[i]->hit())
				{
					
				}
				else
				{
					
				}
				combo++;
				score = score + combo * 10;
			}
			else if (BallLeft(bricks[i]->picture))
			{
				ball.angle = pi - ball.angle;
				ball.setPosition(bricks[i]->picture.getPosition().x + ball.picture.getRadius() + bricks[i]->picture.getSize().x / 2 + 0.1f, ball.picture.getPosition().y);
				if (bricks[i]->hit())
				{
					
				}
				else
				{
					
				}
				combo++;
				score = score + combo * 10;
			}
			else if (BallRight(bricks[i]->picture))
			{
				ball.angle = pi - ball.angle;
				ball.setPosition(bricks[i]->picture.getPosition().x - ball.picture.getRadius() - bricks[i]->picture.getSize().x / 2 - 0.1f, ball.picture.getPosition().y);
				if (bricks[i]->hit())
				{
					
				}
				else
				{
					
				}
				combo++;
				score = score + combo * 10;
			}
		}
	}
	if (life <= 0)
	{
		gameover = true;
		
		gameoverText.setString("Juego acabado, presiona \"Enter\" para reiniciar");
	}

	int count = 0;
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable && bricks[i]->hp < 3)
			count++;
	}

	if (count <= 0)
	{
		win = true;
		ball.speed += 100.f;
		
		gameoverText.setString("Win! press \"Enter\" to next level");
	}
	lifeText.setString("life:" + std::to_string(life));
	scoreText.setString("score:" + std::to_string(score));
}

void Render()
{
	window.clear(sf::Color::Black);
	window.draw(background);
	window.draw(paddle.picture);
	window.draw(ball.picture);

	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			if (bricks[i]->hp == 1)
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(0, 255, 255, 255));
			}
			else if (bricks[i]->hp == 2)
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(255, 0, 0, 255));
			}
			
			window.draw(bricks[i]->picture);
		}

	}
	window.draw(lifeText);
	window.draw(gameoverText);
	window.draw(scoreText);
	window.display();
}

void HandleInput()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			for (int i = 0; i < bricks.size(); ++i)
			{
				delete bricks[i];
				bricks[i] = nullptr;
			}
			bricks.clear();
		}
		else if (event.type == sf::Event::MouseMoved && !gameover && !win)
		{
			if (Mouse::getPosition(window).x < (frameWidth - 100.f) && Mouse::getPosition(window).x  > 100.f)
			{
				paddle.picture.setPosition(Vector2f(event.mouseMove.x, paddle.picture.getPosition().y));
			}
			if (!isPlaying)
			{
				ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
			}
		}
	}



	if (!gameover)
	{
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) &&
			(paddle.picture.getPosition().x - paddle.picture.getSize().x / 2.f > 50.f))
		{
			paddle.picture.move(-paddle.speed * deltaTime, 0.f);
		}
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
			(paddle.picture.getPosition().x + paddle.picture.getSize().x / 2.f < frameWidth - 50.f))
		{
			paddle.picture.move(paddle.speed * deltaTime, 0.f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			isPlaying = true;
		}

		if (!isPlaying)
		{
			ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
		}

	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		if (gameover)
		{
			life = 3;
			gameover = false;
			score = 0;
			combo = 0;
			loadLevel();
			
		}
		else if (win)
		{
			win = false;
			loadLevel();
			
		}
	}
	
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{		
		loadLevel();
	}

}


void loadLevel()
{
	isPlaying = false;
	gameover = false;


	gameoverText.setString("");

	paddle.initiate();
	paddle.setSize(150, 35);
	paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);
	paddle.picture.setTexture(&texturePaddle);

	ball.initiate();
	ball.setSize(10);
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
	ball.picture.setTexture(&textureBall);


	for (int i = 0; i < bricks.size(); ++i)
	{
		delete bricks[i];
		bricks[i] = nullptr;
	}
	bricks.clear();

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			int temp = rand() % 5;
			if (temp == 0)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
				bptr->hp = 1;
				bricks.push_back(bptr);
				}
				else if (temp == 1)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 2;
					bricks.push_back(bptr);
				}
				
				else if (temp == 2)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 1;
					bptr->speed = 300;
					bricks.push_back(bptr);
			}

		}
	}

}

bool BallLeft(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x + ball.picture.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallRight(RectangleShape rect)
{
	if (ball.picture.getPosition().x - ball.picture.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallUp(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallBottom(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}
