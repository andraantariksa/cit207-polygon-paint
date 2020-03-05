#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include <list>

#include <SFML/Graphics.hpp>

#include "shape/common.h"

enum class State
{
	Nothing,
	DrawPolygon
};

class Application
{
private:
	sf::RenderWindow* window_main;
	std::list<sf::Drawable*> shape_layer;
	int width;
	int height;
	State state;

	std::vector<sf::Vertex> polygon_buffer;
public:
	Application(int width, int height, const sf::String& title);
	~Application();

	void update();
	void updateInterface();
	void dispatch();

	void drawPolygon(sf::Event &event);
};

#endif //_APPLICATION_HPP_
