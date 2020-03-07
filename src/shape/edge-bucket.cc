#include <cmath>

#include "edge-bucket.h"

namespace shape
{

	bool EdgeBucket::next(int y_pos)
	{
		// Frac
		// dx    1
		// -- = ---
		// dy    m
		carry += std::abs(dx);
		while (2 * carry >= dy)
		{
			carry -= dy;
			if (dx > 0)
			{
				x_of_y_min++;
			}
			else
			{
				x_of_y_min--;
			}
		}

		return y_pos < y_max;
	}

}
