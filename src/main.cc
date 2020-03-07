#include <SFML/System.hpp>

#include "application.h"

int main()
{
	Application app(1000, 600, sf::String("Polygon Paint"));
	app.dispatch();
}
