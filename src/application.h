#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include "shape/layer.h"
#include "shape/polygon.h"

#include <list>

#include <SFML/Graphics.hpp>

enum class State
{
	Nothing,
	DrawPolygon,
	EditVertexPolygon
};

class Application
{
private:
	sf::RenderWindow* window_main;
	// TODO
	// Make the shape become general, not only polygon
	std::vector<Layer> layers;
	int width;
	int height;
	State state;
	int layer_counter;

	float picked_color_primary[3];
	float picked_color_secondary[3];
	int selected_layer_idx;
	int selected_fill_color_choice;

	shape::Polygon* current_polygon_buffer;
	sf::Vertex* vertex_buffer;
	bool mouse_hold;
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
	void updateInterface(Assets& assets);
	void dispatch();

	void editVertexPolygonEvent(sf::Event& event);
	void endVertexPolygonEvent();
	void drawPolygonEvent(sf::Event& event);
	void endDrawPolygonEvent();
};

#endif //_APPLICATION_HPP_
