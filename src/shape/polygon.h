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
//		Maximum distance to find the nearest vertex
//		Related with getNearestVertex()
		const int vertex_edit_drag_max_distance = 10;
//		The rectangle color in vertex when editing
		const sf::Color vertex_edit_color = sf::Color::Black;

		std::vector<sf::Vertex> vertices;
//		Has finish the vertices
//		If true, it will draw the end edges (from end vertex to begin vertex)
		bool finish;
//		Color
		sf::Color color_outline;
		sf::Color color_fill;
		bool is_filled;
//		Edit mode
		bool edit_mode;

		struct SortedEdgeTable
		{
			int y_min;
			std::vector<std::vector<EdgeBucket>*> lines;
		}
			sorted_edge_table;
	public:
		Polygon();
		Polygon(float picked_color_primary[3], float picked_color_secondary[3], bool is_filled);
		Polygon(Polygon const& another_polygon);

		void appendVertex(sf::Vertex vertex);
		sf::Vertex popBackVertex();
		void endVertex();

		int size();
		const sf::Color& getFillColor();
		void setFillColor(const float color[3]);
		void setOutlineColor(const float color[3]);
		void setFillColor(sf::Color& color);
		void setOutlineColor(sf::Color& color);

		const sf::Color& getOutlineColor();

		const bool& isFilled();
		void isFilled(bool is_it);

		const std::vector<sf::Vertex>& data();

		SortedEdgeTable constructSortedEdgeTable();
		// Switch edit mode of polygon
		void isEditMode(bool is_it);
		// Get nearest vertex from a position with a certain constraint
		sf::Vertex* getNearestVertex(sf::Vector2i pos);
		void removeNearestVertex(sf::Vector2i pos);

		void fill(sf::RenderTarget& window) const;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

#ifdef DEBUG
		static void printSortedEdgeTable(SortedEdgeTable& sorted_edge_table);
#endif
	};

}

#endif //_POLYGON_H_
