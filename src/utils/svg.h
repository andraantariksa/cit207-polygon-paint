#ifndef _SVG_H_
#define _SVG_H_

#include "../../lib/simple-svg/simple_svg.hpp"
#include "../shape/layer.h"

namespace utils
{

	class SVG
	{
	private:
		svg::Dimensions dimension;
		svg::Document document;

	public:
		SVG(int width, int height, char const* filename);

		void from(std::vector<Layer>& shapes);
		void save();
	};

}

#endif //_SVG_H_
