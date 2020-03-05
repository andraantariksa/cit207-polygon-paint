#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <vector>
#include <list>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace shape
{
	struct EdgeBucket
	{
		int y_max;
		int x_of_y_min;
		int dx;
		int dy;
		int carry;
	};

	class Polygon : public sf::Drawable
	{
	private:
		std::list<sf::Vertex> vertexes;
		sf::Color color_border;
		sf::Color color_fill;
		bool is_filled;
	public:
		struct SortedEdgeTable
		{
			std::vector<std::list<EdgeBucket>*> lines;
			int y_min;
		};

		Polygon();

		// From drawable
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

		void appendVertex(sf::Vertex vertex);
		sf::Vertex popBackVertex();
		SortedEdgeTable constructSortedEdgeTable();
		void fill(SortedEdgeTable &sorted_edge_table, sf::RenderWindow* window);

#ifdef DEBUG
		static void printSortedEdgeTable(SortedEdgeTable &sorted_edge_table);
#endif
	};

}

#endif //_POLYGON_H_
