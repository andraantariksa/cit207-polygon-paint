#define DEBUG

#include "application.cc"

int main()
{
	Application app(1000, 600, sf::String("Hexagon Paint"));
	app.dispatch();
}
