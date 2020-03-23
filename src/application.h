#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include "shape/layer.h"
#include "shape/polygon.h"
#include "state-manager.h"
#include "event-manager.h"

#include <list>
#include <memory>

#include <SFML/Graphics.hpp>

class Application
{
private:
	// Window
	std::unique_ptr<sf::RenderWindow> window_main;
	int window_width;
	int window_height;

	// Layers
	std::vector<Layer> layers;
	int layer_counter;

	// State and event
	StateManager state_manager;
	EventManager event_manager;

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

	// Render the layers
	void update();
	// Render the interface
	void updateInterface(Assets& assets);
	// Main loop
	void dispatch();

	void editVertexPolygonEvent(sf::Event& event);
	void drawPolygonEvent(sf::Event& event);
};

#endif //_APPLICATION_HPP_
