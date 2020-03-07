#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <vector>
#include <list>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "edge-bucket.h"

namespace shape
{
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
		Polygon(bool is_filled);
		Polygon(Polygon const &another_polygon);

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
