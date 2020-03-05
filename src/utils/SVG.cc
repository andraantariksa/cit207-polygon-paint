#include "SVG.h"

#include "../../lib/simple-svg/simple_svg.hpp"

namespace utils
{

	SVG::SVG(int width, int height, char* filename)
		:
		dimension(width, height),
		document(filename, svg::Layout(dimension, svg::Layout::TopLeft))
	{
	}

	void SVG::from(std::vector<sf::Drawable*>& shapes)
	{
		// TODO
		// Make the shape become general, not only polygon
		svg::Point point;
		for (auto shape : shapes)
		{
			svg::Polygon polygon(svg::Stroke(1, svg::Color::Black));
			for (auto vertex : ((shape::Polygon*)shape)->data())
			{
				point.x = vertex.position.x;
				point.y = vertex.position.y;
				polygon << point;
			}

			document << polygon;
		}
	}

	void SVG::save()
	{
		document.save();
	}

};
