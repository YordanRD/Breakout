#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <iostream>


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

int life = 1;
int score = 0;
int deep = 0;

int paddleSize;

const float startposX = 55;
const float startposY = 70;

Paddle paddle;

Texture textureBall;
RectangleShape background;
Texture textureBack;
Texture texturePaddle;
Texture textureBrick;

vector<Brick*> bricks;
vector<Ball*> balls;

void Initiate();
void Reset();
void Update();
void Render();
void HandleInput();
void loadLevel();

bool BallLeft(RectangleShape rect, CircleShape circ);
bool BallRight(RectangleShape rect, CircleShape circ);
bool BallUp(RectangleShape rect, CircleShape circ);
bool BallBottom(RectangleShape rect, CircleShape circ);

int main()
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	sf::TcpSocket socket;

	char connectionType, mode;

	std::cout << "(s) si es servidor, (c) si es cliente" << std::endl;
	cin >> connectionType;

	if (connectionType == 's') {

		srand((unsigned)(time(NULL)));

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

		sf::TcpListener listener;
		listener.listen(2000);
		listener.accept(socket);
	}

	else if (connectionType == 'c') {
		socket.connect(ip, 2000);
	}

	return EXIT_SUCCESS;
}

/**
* Inicializa las imagenes y textos necesarios para el correcto funcionamiento del juego
*/
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
	lifeText.setString("Vida:" + std::to_string(life));

	gameoverText.setFont(font);
	gameoverText.setCharacterSize(35);
	gameoverText.setPosition(100, 400);
	gameoverText.setString("");

	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setPosition(80, frameHeight - 30);
	scoreText.setString("Puntuaci�n:" + std::to_string(score));

}

/***/
void Reset()
{
	for (int i = 0; i < balls.size(); i++) {
		balls[i]->setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - balls[i]->picture.getRadius());
		balls[i]->angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
	}

}

/**
* M�todo que actualiza el estado de las partes del juego en pantalla
* En caso de que la bola no est� colisionando con nada la hace seguir su movimiento
* En caso de chocar con la barra del jugador cambia su direcci�n y angulo
* En caso de chocar con un borde cambia su dirrecci�n y angulo
* En caso de chocar con un bloque, baja la cantidad de golpes faltantes para que este se destruya
* Si una bola colisiona con el borde inferior, esta es eliminada
*/
void Update()
{
	for (int i = 0; i < balls.size(); i++) {
		if (balls[i]->angle < 0)
		{
			balls[i]->angle += 2 * pi;
		}
		balls[i]->angle = fmod(balls[i]->angle, 2 * pi);

		
		float factor = balls[i]->speed * deltaTime;
		balls[i]->picture.move(std::cos(balls[i]->angle) * factor, std::sin(balls[i]->angle) * factor);
		//Fisicas
		//Bordes
		if (balls[i]->picture.getPosition().y + balls[i]->picture.getRadius() > frameHeight)
		{
			if (balls.size() == 1) {
				isPlaying = false;
				life--;

			}
			else {
				life--;
				paddleSize = paddleSize - 30;
				paddle.setSize(paddleSize, 35);
				balls.erase(balls.begin() + i);
				std::cout << "Balls left: " << balls.size() << endl;
			}
			break;
		}
		else if (balls[i]->picture.getPosition().x - balls[i]->picture.getRadius() < 50.f)
		{
			balls[i]->angle = pi - balls[i]->angle;
			balls[i]->picture.setPosition(balls[i]->picture.getRadius() + 50.1f, balls[i]->picture.getPosition().y);

		}
		else if (balls[i]->picture.getPosition().x + balls[i]->picture.getRadius() > frameWidth - 50)
		{
			balls[i]->angle = pi - balls[i]->angle;
			balls[i]->setPosition(frameWidth - balls[i]->picture.getRadius() - 50.1f, balls[i]->picture.getPosition().y);

		}
		else if (balls[i]->picture.getPosition().y - balls[i]->picture.getRadius() < 50.f)
		{
			balls[i]->angle = -balls[i]->angle;
			balls[i]->setPosition(balls[i]->picture.getPosition().x, balls[i]->picture.getRadius() + 50.1f);

		}


		//Plataforma
		if (BallBottom(paddle.picture, balls[i]->picture))
		{
			int dis = balls[i]->picture.getPosition().x - paddle.picture.getPosition().x;
			if (dis > 30 && balls[i]->angle > 1.f / 2.f * pi)
			{
				balls[i]->angle = balls[i]->angle - pi;
			}
			else if (dis < -30 && balls[i]->angle < 1.f / 2.f * pi)
			{
				balls[i]->angle = balls[i]->angle - pi;
			}
			else
			{
				balls[i]->angle = -balls[i]->angle;
				if (balls[i]->angle > 1.f / 2.f * pi && balls[i]->angle < 7.f / 8.f * pi)
				{
					balls[i]->angle += (std::rand() % 15) * pi / 180;
				}
				else if (balls[i]->angle < 1.f / 2.f * pi && balls[i]->angle > 1.f / 8.f * pi)
				{
					balls[i]->angle -= (std::rand() % 15) * pi / 180;
				}
				else if (balls[i]->angle <= 1.f / 8.f * pi)
				{
					balls[i]->angle += (std::rand() % 15) * pi / 180;
				}
				else if (balls[i]->angle >= 7.f / 8.f * pi)
				{
					balls[i]->angle -= (std::rand() % 15) * pi / 180;
				}
			}


			balls[i]->setPosition(balls[i]->picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - balls[i]->picture.getRadius() - 0.1f);

		}
		//Bloques
		
		for (int j = 0; j < bricks.size(); ++j)
		{	
			if (bricks[j]->enable)
			{
				if (BallUp(bricks[j]->picture, balls[i]->picture))
				{	
					if (deep != 0) {
						balls[i]->setPosition(balls[i]->picture.getPosition().x, bricks[j]->picture.getPosition().y - bricks[j]->picture.getSize().y / 2 - balls[i]->picture.getRadius() - 0.1f);
						deep--;
					}

					else if (deep == 0) {
						balls[i]->angle = -balls[i]->angle;
						balls[i]->setPosition(balls[i]->picture.getPosition().x, bricks[j]->picture.getPosition().y + bricks[j]->picture.getSize().y / 2 + balls[i]->picture.getRadius() + 0.1f);
						if (bricks[j]->hit())
						{
							// Primer rebote
							int x;
							if (balls.size() == 3 && life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2 && possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else if (balls.size() == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2) {
										x = possX;
										break;
									}
								}
							}
							else if (life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else {
								x = rand() % 10;
							}

							if (x == 0) {
								life++;
								paddleSize = paddleSize + 30;
								paddle.setSize(paddleSize, 35);
							}

							else if (x == 1) {
								balls[i]->speed = 1000.f;
							}
							else if (x == 2) {
								Ball* ball = new Ball;
								ball->initiate();
								ball->setSize(10);
								ball->setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball->picture.getRadius());
								ball->angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
								ball->speed = 500.f;
								ball->picture.setTexture(&textureBall);
								balls.push_back(ball);
							}
							else if (x == 3) {
								balls[i]->speed = 500.f;
							}
							else if (x == 4) {
								balls[i]->speed = 200.f;
							}
							else if (x == 5) {
								paddle.setSize(210, 35);
							}
							else if (x == 6) {
								paddle.setSize(150, 35);
							}
							else if (x == 7) {
								paddle.setSize(75, 35);
							}
							else if (x == 8) {
								deep = 3;
							}

							cout << "Xvalue: " << x << endl;
							score = score + 10;
						}
						else
						{
							score = score + 5;
						}

					}
					
				}
				else if (BallBottom(bricks[j]->picture, balls[i]->picture))
				{	
					if (deep != 0) {
						balls[i]->setPosition(balls[i]->picture.getPosition().x, bricks[j]->picture.getPosition().y + bricks[j]->picture.getSize().y / 2 + balls[i]->picture.getRadius() + 0.1f);
						deep--;
					}
					else if (deep == 0) {
						balls[i]->angle = -balls[i]->angle;
						balls[i]->setPosition(balls[i]->picture.getPosition().x, bricks[j]->picture.getPosition().y - bricks[j]->picture.getSize().y / 2 - balls[i]->picture.getRadius() - 0.1f);
						if (bricks[j]->hit())
						{
							int x;
							if (balls.size() == 3 && life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2 && possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else if (balls.size() == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2) {
										x = possX;
										break;
									}
								}
							}
							else if (life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else {
								x = rand() % 10;
							}
							if (x == 0) {
								life++;
								paddleSize = paddleSize + 30;
								paddle.setSize(paddleSize, 35);
							}

							else if (x == 1) {
								balls[i]->speed = 1000.f;
							}
							else if (x == 2) {
								Ball* ball = new Ball;
								ball->initiate();
								ball->setSize(10);
								ball->setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball->picture.getRadius());
								ball->angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
								ball->speed = 500.f;
								ball->picture.setTexture(&textureBall);
								balls.push_back(ball);
							}
							else if (x == 3) {
								balls[i]->speed = 500.f;
							}
							else if (x == 4) {
								balls[i]->speed = 200.f;
							}
							else if (x == 5) {
								paddle.setSize(210, 35);
							}
							else if (x == 6) {
								paddle.setSize(150, 35);
							}
							else if (x == 7) {
								paddle.setSize(75, 35);
							}
							else if (x == 8) {
								deep = 3;
							}

							cout << "XValue: " << x << endl;
							score = score + 10;

						}
						else
						{
							score = score + 5;
						}
					}
				}
				else if (BallLeft(bricks[j]->picture, balls[i]->picture))
				{	
					if (deep == 0) {
						balls[i]->angle = pi - balls[i]->angle;
						balls[i]->setPosition(bricks[j]->picture.getPosition().x - balls[i]->picture.getRadius() - bricks[j]->picture.getSize().x / 2 + 0.1f, balls[i]->picture.getPosition().y);
						if (bricks[i]->hit())
						{
							int x;
							if (balls.size() == 3 && life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2 && possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else if (balls.size() == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2) {
										x = possX;
										break;
									}
								}
							}
							else if (life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else {
								x = rand() % 10;
							}
							if (x == 0) {
								life++;
								paddleSize = paddleSize + 30;
								paddle.setSize(paddleSize, 35);
							}

							else if (x == 1) {
								balls[i]->speed = 1000.f;
							}
							else if (x == 2) {
								Ball* ball = new Ball;
								ball->initiate();
								ball->setSize(10);
								ball->setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball->picture.getRadius());
								ball->angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
								ball->speed = 500.f;
								ball->picture.setTexture(&textureBall);
								balls.push_back(ball);
							}
							else if (x == 3) {
								balls[i]->speed = 500.f;
							}
							else if (x == 4) {
								balls[i]->speed = 200.f;
							}
							else if (x == 5) {
								paddle.setSize(210, 35);
							}
							else if (x == 6) {
								paddle.setSize(150, 35);
							}
							else if (x == 7) {
								paddle.setSize(75, 35);
							}
							else if (x == 8) {
								deep = 3;
							}



							cout << i << endl;
							score = score + 10;
						}
						else
						{
							score = score + 5;
						}
					}
				}
				else if (BallRight(bricks[j]->picture, balls[i]->picture))
				{	
					if (deep == 0) {
						balls[i]->angle = pi - balls[i]->angle;
						balls[i]->setPosition(bricks[j]->picture.getPosition().x + balls[i]->picture.getRadius() + bricks[j]->picture.getSize().x / 2 - 0.1f, balls[i]->picture.getPosition().y);
						if (bricks[i]->hit())
						{
							int x;
							if (balls.size() == 3 && life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2 && possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else if (balls.size() == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 2) {
										x = possX;
										break;
									}
								}
							}
							else if (life == 3) {
								while (true) {
									int possX = rand() % 10;
									if (possX != 0) {
										x = possX;
										break;
									}
								}
							}
							else {
								x = rand() % 10;
							}
							if (x == 0) {
								life++;
								paddleSize = paddleSize + 30;
								paddle.setSize(paddleSize, 35);
							}

							else if (x == 1) {
								balls[i]->speed = 1000.f;
							}
							else if (x == 2) {
								Ball* ball = new Ball;
								ball->initiate();
								ball->setSize(10);
								ball->setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball->picture.getRadius());
								ball->angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
								ball->speed = 500.f;
								ball->picture.setTexture(&textureBall);
								balls.push_back(ball);
							}
							else if (x == 3) {
								balls[i]->speed = 500.f;
							}
							else if (x == 4) {
								balls[i]->speed = 200.f;
							}
							else if (x == 5) {
								paddle.setSize(210, 35);
							}
							else if (x == 6) {
								paddle.setSize(150, 35);
							}
							else if (x == 7) {
								paddle.setSize(75, 35);
							}
							else if (x == 8) {
								deep = 3;
							}


							cout << i << endl;
							score = score + 10;
						}
						else
						{
							score = score + 5;
						}
					}
				}
			}
		}

		if (life <= 0)
		{
			gameover = true;

			gameoverText.setString("Juego acabado, presiona \n \"Enter \" \n para reiniciar");
		}

		int count = 0;
		for (int i = 0; i < bricks.size(); ++i)
		{
			if (bricks[i]->enable && bricks[i]->hp < 4)
				count++;
		}

		if (count <= 0)
		{
			win = true;
			balls[i]->speed += 100.f;

			gameoverText.setString("Ganó! presione \"Enter\" para reiniciar el juego");
		}
		lifeText.setString("Vida:" + std::to_string(life));
		scoreText.setString("Puntuación:" + std::to_string(score));
	}
	
}

/**
* Actualiza los elementos graficos de cada parte del juego, bloques, las bolas y la barra del jugador
* As� como el puntaje una ves que los datos son actualizados en Update
*/
void Render()
{
	window.clear(sf::Color::Black);
	window.draw(background);
	window.draw(paddle.picture);
	for (int i = 0; i < balls.size(); ++i)
	{
		window.draw(balls[i]->picture);
	}

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
			else if (bricks[i]->hp == 3)
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(255, 0, 255, 255));
			}

			window.draw(bricks[i]->picture);
		}

	}
	window.draw(lifeText);
	window.draw(gameoverText);
	window.draw(scoreText);
	window.display();
}

/**Se encarga de recibir las se�ales de las teclas o el mouse utilizadas por el jugador para poder 
* interactuar con el juego 
*/
void HandleInput()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			for (int i = 0; i < balls.size(); ++i)
			{
				delete balls[i];
				balls[i] = nullptr;
			}
			balls.clear();

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
				for (int i = 0; i < balls.size(); i++) {
					balls[i]->picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - balls[i]->picture.getRadius());
				}
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
			for (int i = 0; i < balls.size(); i++) {
				balls[i]->picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - balls[i]->picture.getRadius());
			}
		}

	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		if (gameover)
		{
			life = 3;
			gameover = false;
			score = 0;

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

/**
* Se encarga de dar valores a cada una de las variables o atributos importantes que requiere el juego
* Se encarga de posicionar las partes del juego para que sean renderizadas y presentadas al usuario.
*/
void loadLevel()
{
	isPlaying = false;
	gameover = false;


	gameoverText.setString("");

	paddleSize = 100;
	paddle.initiate();
	paddle.setSize(paddleSize, 35);
	paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);
	paddle.picture.setTexture(&texturePaddle);

	for (int i = 0; i < balls.size(); ++i)
	{
		delete balls[i];
		balls[i] = nullptr;
	}
	balls.clear();

	//Se inicializa la bola y se a�ade a la lista de bolas
	Ball* ball = new Ball();
	ball->initiate();
	ball->setSize(10);
	ball->setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball->picture.getRadius());
	ball->angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
	ball->speed = 500.f;
	ball->picture.setTexture(&textureBall);
	balls.push_back(ball);


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
				bptr->hp = 3;
				bricks.push_back(bptr);
			}


		}
	}

}

/**Determina si una bola esta colisionando con un bloque o con la barra en un punto de tangencia ubicado en su superficie izquierdo*/
bool BallLeft(RectangleShape rect, CircleShape circ)
{
	if (circ.getPosition().x + circ.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		circ.getPosition().x + circ.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		circ.getPosition().y + circ.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		circ.getPosition().y - circ.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}

/**Determina si una bola esta colisionando con un bloque o con la barra en un punto de tangencia ubicado en su superficie derecha*/
bool BallRight(RectangleShape rect, CircleShape circ)
{
	if (circ.getPosition().x - circ.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		circ.getPosition().x - circ.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		circ.getPosition().y + circ.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		circ.getPosition().y - circ.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}

/**Determina si una bola esta colisionando con un bloque o con la barra en un punto de tangencia ubicado en su superficie superior*/
bool BallUp(RectangleShape rect, CircleShape circ)
{
	if (circ.getPosition().x + circ.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		circ.getPosition().x - circ.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		circ.getPosition().y - circ.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		circ.getPosition().y - circ.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}

/**Determina si una bola esta colisionando con un bloque o con la barra en un punto de tangencia ubicado en su superficie inferior*/
bool BallBottom(RectangleShape rect, CircleShape circ)
{
	if (circ.getPosition().x + circ.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		circ.getPosition().x - circ.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		circ.getPosition().y + circ.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		circ.getPosition().y + circ.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}