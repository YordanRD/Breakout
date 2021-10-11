#pragma once
#include <SFML/Graphics.hpp>

class Ball
{
public:
	float speed = 500.f;
	float angle;
	sf::CircleShape picture;

	/**@brief
	* 
	* Se inicializa la pelota dandole un color de fondo.
	*/
	void initiate();

	/**@brief
	*
	* Se le determina un tamano y posicion inicial a la pelota.
	*/
	void setSize(float radius);

	/**@brief
	*
	* Actualiza la posicion vertical y horizontal de la pelota.
	*/
	void setPosition(float x, float y);
};


