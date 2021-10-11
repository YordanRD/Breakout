#include "Ball.h"


void Ball::initiate()
{
	picture.setFillColor(sf::Color::White);
}

/**@brief
* Se le determina un tamaño y posicion inicial a la pelota
*/
void Ball::setSize(float radius)
{
	picture.setRadius(radius);
	picture.setOrigin(radius / 2.f, radius / 2.f);
}

/**@brief
* Actualiza la posicion vertical y horizontal de la pelota
*/
void Ball::setPosition(float x, float y)
{
	picture.setPosition(sf::Vector2f(x, y));
}