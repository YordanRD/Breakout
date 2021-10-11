#pragma once
#include <SFML/Graphics.hpp>

class Brick
{
public:
	bool enable = true;
	bool moveLeft = true;
	bool destroyed = true;

	int hp = 1;
	float speed = 0;
	sf::RectangleShape picture;

	/**@brief
	*
	* Se inicializa y da un color de fondo al bloque.
	*/
	void initiate();

	/**@brief
	*
	* Se le asigna un tamano y origen al bloque.
	*/
	void setSize(float width, float height);

	/**@brief
	*
	* Se le asigna la posicion a los bloques en pantalla.
	*/
	void setPosition(float x, float y);

	/**@brief
	*
	*	Detecta cuando la pelota entra en contacto con un bloque y reduce la vida del bloque.
	*
	*	TRUE Avisa que la vida del bloque es 0 y se destruyo
	*	FALSE Indica que el bloque aun tiene vida
	*/
	virtual bool hit();
};

