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
	// TODO
	// Make the shape become general, not only polygon
	std::vector<sf::Drawable*> shape_layers;
	int width;
	int height;
	State state;

	float picked_color[4];
	int selected_layer_idx;

	shape::Polygon *current_polygon_buffer;
public:
	struct Assets
	{
		struct
		{
			sf::Texture polygon;
		}
		icon;
	};

	Application(int width, int height, const sf::String& title);
	~Application();

	void update();
	void updateInterface(Assets &assets);
	void dispatch();

	void drawPolygonEvent(sf::Event &event);
	void endDrawPolygonEvent();
};

#endif //_APPLICATION_HPP_
