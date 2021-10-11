#include "Paddle.h"

/**@brief
*
* Se inicializa y da un color de fondo a la plataforma
*/
void Paddle::initiate()
{
	picture.setFillColor(sf::Color::White);
}

/**@brief
*
* Se le asigna un tamano a la plataforma
*/
void Paddle::setSize(float width, float height)
{
	picture.setSize(sf::Vector2f(width, height));
	picture.setOrigin(picture.getSize() / 2.f);
}

/**@brief
*
* Se actualiza la posicion de la plataforma
*/
void Paddle::setPosition(float x, float y)
{
	picture.setPosition(sf::Vector2f(x, y));
}