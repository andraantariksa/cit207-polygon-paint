#ifndef _LAYER_H_
#define _LAYER_H_

#include <memory>

#include "polygon.h"

enum class LayerObjectType
{
	Polygon
};

union Objects
{
	shape::Polygon *polygon;
};

class Layer
{
public:
	LayerObjectType type;
	std::string name;
	Objects object;

	Layer();
	Layer(Layer const &another_layer);

	void draw(sf::RenderWindow* window);
	void free();
};

#endif //_LAYER_H_
