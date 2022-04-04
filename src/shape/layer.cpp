#include <stdexcept>
#include <cstdio>
#include <SFML/Graphics/RenderWindow.hpp>

#include "layer.h"

Layer::Layer()
{
}

Layer::Layer(Layer const &another_layer)
	:
	type(another_layer.type),
	name(another_layer.name),
	object(another_layer.object)
{
}

void Layer::draw(sf::RenderWindow *window)
{
	if (type == LayerObjectType::Polygon)
	{
		window->draw(*object.polygon);
	}
	else
	{
		throw std::logic_error("Not implemented");
	}
}

void Layer::free()
{
	if (type == LayerObjectType::Polygon)
	{
		delete object.polygon;
	}
	else
	{
		throw std::logic_error("Not implemented");
	}
}
