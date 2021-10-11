#pragma once
#include <SFML/Graphics.hpp>

class Paddle
{
public:
	float speed = 1000.f;
	sf::RectangleShape picture;

	/**@brief
	*
	* Se inicializa y da un color de fondo a la plataforma
	*/
	void initiate();

	/**@brief
	*
	* Se le asigna un tamano a la plataforma
	*/
	void setSize(float width, float height);

	/**@brief
	*
	* Se actualiza la posicion de la plataforma
	*/
	void setPosition(float x, float y);
};

