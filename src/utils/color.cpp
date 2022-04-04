#include "color.h"

sf::Color color_float3(const float color[3])
{
	return sf::Color(
		(sf::Uint8) (color[0] * 255.0),
		(sf::Uint8) (color[1] * 255.0),
		(sf::Uint8) (color[2] * 255.0)
	);
}
