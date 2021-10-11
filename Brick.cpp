#include "Brick.h"

/**@brief
*
* Se inicializa y da un color de fondo al bloque.
*/
void Brick::initiate()
{
	picture.setFillColor(sf::Color::White);
}

/**@brief
*
* Se le asigna un tamano y origen al bloque.
*/
void Brick::setSize(float width, float height)
{
	picture.setSize(sf::Vector2f(width, height));
	picture.setOrigin(picture.getSize() / 2.f);
}

/**@brief
*
* Se le asigna la posicion a los bloques en pantalla.
*/
void Brick::setPosition(float x, float y)
{
	picture.setPosition(sf::Vector2f(x, y));
}

/**@brief
*
*Detecta cuando la pelota entra en contacto con un bloque y reduce la vida del bloque.
* 
*@retval TRUE	Avisa que la vida del bloque es 0 y se destruyo
*@retval FALSE	Indica que el bloque aun tiene vida
*/
bool Brick::hit()
{
	
	hp--;
	if (hp == 0)
	{
		enable = false;
		return true;
	}
	else
	{
		return false;
	}
	
}