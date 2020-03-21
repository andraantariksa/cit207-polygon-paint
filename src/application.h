#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include "shape/layer.h"
#include "shape/polygon.h"
#include "state.h"

#include <list>
#include <memory>

#include <SFML/Graphics.hpp>

class Application
{
private:
	std::unique_ptr<sf::RenderWindow> window_main;
	int window_width;
	int window_height;

	std::vector<Layer> layers;
	StateManager state_manager;
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

	Application(int window_width, int window_height, const sf::String& title);
	~Application();

	void update();
	void updateInterface(Assets& assets);
	void dispatch();

	void editVertexPolygonEvent(sf::Event& event);
	void endEditVertexPolygonEvent();
	void drawPolygonEvent(sf::Event& event);
	void endDrawPolygonEvent();
};

#endif //_APPLICATION_HPP_
