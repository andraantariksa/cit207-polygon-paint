#ifndef _POLYGON_H_
#define _POLYGON_H_

//#define DEBUG
//#define VERBOSE 0

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
		sf::Color color_outline;
		sf::Color color_fill;
		bool is_filled;
		bool edit_mode;
	public:
		struct SortedEdgeTable
		{
			int y_min;
			std::vector<std::vector<EdgeBucket>*> lines;
		}
			sorted_edge_table;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		Polygon();
		Polygon(float picked_color_primary[3], float picked_color_secondary[3], bool is_filled);
		Polygon(Polygon const& another_polygon);

		void appendVertex(sf::Vertex vertex);
		void endVertex();
		sf::Vertex popBackVertex();
		int size();
		const sf::Color& getFillColor();
		const sf::Color& getOutlineColor();
		const bool& isFilled();
		const std::vector<sf::Vertex>& data();

		SortedEdgeTable constructSortedEdgeTable();
		void fill(sf::RenderTarget& window) const;
		void startEditMode();
		void endEditMode();
		sf::Vertex* getNearestVertex(sf::Vector2i pos);

#ifdef DEBUG
		static void printSortedEdgeTable(SortedEdgeTable& sorted_edge_table);
#endif
	};

}

#endif //_POLYGON_H_
