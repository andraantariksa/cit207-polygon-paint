#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <vector>
#include <list>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace shape
{
	class EdgeBucket
	{
	private:
	public:
		int y_max;
		int x_of_y_min;
		int dx;
		int dy;
		int carry;
		bool next(int pos);
	};

	class Polygon : public sf::Drawable
	{
	private:
		std::vector<sf::Vertex> vertexes;
		sf::Color color_border;
		sf::Color color_fill;
		bool is_filled;
	public:
		struct SortedEdgeTable
		{
			int y_min;
			std::vector<std::vector<EdgeBucket>*> lines;
		};

		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

		Polygon();

		void appendVertex(sf::Vertex vertex);
		void endVertex();
		sf::Vertex popBackVertex();
		int size();
		const std::vector<sf::Vertex>& data();

		SortedEdgeTable constructSortedEdgeTable();
		void fill(SortedEdgeTable &sorted_edge_table, sf::RenderWindow* window);

#ifdef DEBUG
		static void printSortedEdgeTable(SortedEdgeTable &sorted_edge_table);
#endif
	};

}

#endif //_POLYGON_H_
