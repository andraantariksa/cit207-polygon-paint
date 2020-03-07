#include <SFML/Graphics/Drawable.hpp>

#include "../../lib/simple-svg/simple_svg.hpp"

#include "svg.h"
#include "../shape/polygon.h"
#include "../shape/layer.h"

namespace utils
{

	SVG::SVG(int width, int height, char const* filename)
		:
		dimension(width, height),
		document(filename, svg::Layout(dimension, svg::Layout::TopLeft))
	{
	}

	void SVG::from(std::vector<Layer>& layers)
	{
		// TODO
		// Make the shape become general, not only polygon
		svg::Point point;
		for (auto layer : layers)
		{
			if (layer.type == LayerObjectType::Polygon)
			{
				svg::Polygon polygon(svg::Stroke(1, svg::Color::Black));
				for (auto vertex : ((shape::Polygon*) layer.object.polygon)->data())
				{
					point.x = vertex.position.x;
					point.y = vertex.position.y;
					polygon << point;
				}

				document << polygon;
			}
			else
			{
				throw std::logic_error("Not implemented");
			}
		}
	}

	void SVG::save()
	{
		document.save();
	}

};
