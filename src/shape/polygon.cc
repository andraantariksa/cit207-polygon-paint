#include <algorithm>

#include "polygon.h"

namespace shape
{
	Polygon::Polygon()
	{
	}

	void Polygon::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		sf::VertexArray vertex_array(sf::LineStrip, vertexes.size());
		int i = 0;
		for (auto vertex : vertexes)
		{
			vertex_array[i] = vertex;
			i++;
		}
		target.draw(vertex_array);
	}

	void Polygon::appendVertex(sf::Vertex vertex)
	{
		vertexes.push_back(vertex);
	}

	sf::Vertex Polygon::popBackVertex()
	{
		sf::Vertex ret = vertexes.back();
		vertexes.pop_back();
		return ret;
	}

	Polygon::SortedEdgeTable Polygon::constructSortedEdgeTable()
	{
		// Next vertex iterator
		std::list<sf::Vertex>::iterator next_vertex;

		EdgeUsedBySortedEdgeTable temp{};
		temp.carry = 0; // Because the carry is always 0

		// Used for sorting the vertex by y value
		// the vertex with y-minimum would be the in the 1st element (Index 0)
		// and vice-versa
		sf::Vector2f sorted_by_y_vertex[2];

		// Get the size of table
		auto [vertex_with_y_min, vertex_with_y_max] = std::minmax_element(vertexes.begin(), vertexes.end(), [](const sf::Vertex a, const sf::Vertex b){
			return a.position.y > b.position.y;
		});
		int table_size = (int) vertex_with_y_min->position.y - (int) vertex_with_y_max->position.y + 1;

		Polygon::SortedEdgeTable sorted_edge_table(table_size);

		// For loop from beginning to end - 1, and taking 2 element each
		// [1, 2], [2, 3], ...
		for (auto current_vertex = vertexes.begin(); current_vertex != std::prev(vertexes.end()); ++current_vertex)
		{
			next_vertex = std::next(current_vertex);

			// Whatever, the assignment on which index is doesn't matter
			sorted_by_y_vertex[0] = current_vertex->position;
			sorted_by_y_vertex[1] = next_vertex->position;

			std::sort(
				sorted_by_y_vertex,
				sorted_by_y_vertex + 1,
				[](const sf::Vector2f a, const sf::Vector2f b){
					return a.y > b.y;
			});

			int y_min = (int) sorted_by_y_vertex[0].y;
			temp.y_max = (int) sorted_by_y_vertex[1].y;
			temp.x_of_y_min = (int) sorted_by_y_vertex[0].x;

			// Delta
			temp.dx = (int) next_vertex->position.x - (int) current_vertex->position.x;
			temp.dy = (int) next_vertex->position.y - (int) current_vertex->position.y;

			if (sorted_edge_table[(int) vertex_with_y_min->position.y - y_min] == nullptr)
			{
				sorted_edge_table[(int) vertex_with_y_min->position.y - y_min] = new std::list<EdgeUsedBySortedEdgeTable>();
			}

			sorted_edge_table[(int) vertex_with_y_min->position.y - y_min]->push_back(temp);
		}
		return sorted_edge_table;
	}

	void Polygon::fill(Polygon::SortedEdgeTable &sorted_edge_table, sf::RenderWindow* window)
	{
		for (auto current_sorted_edge_table_element : sorted_edge_table)
		{

		}
	}

#ifdef DEBUG
	void Polygon::printSortedEdgeTable(Polygon::SortedEdgeTable &sorted_edge_table)
	{
		printf("DEBUG SET\n");
		printf("SET size is %zu\n", sorted_edge_table.size());

		int i = -1;
		for (std::list<EdgeUsedBySortedEdgeTable>* current_sorted_edge_table_element : sorted_edge_table)
		{
			i++;
			if (current_sorted_edge_table_element == nullptr)
			{
				continue;
			}
			printf("%d.\n", i);
			for (EdgeUsedBySortedEdgeTable edge : *current_sorted_edge_table_element)
			{
				printf("  -> Edge ymax=%d, y_min=%d, dx=%d, dy=%d, carry=%d\n", edge.y_max, edge.x_of_y_min, edge.dx, edge.dy, edge.carry);
			}
			printf("\n");
		}
		printf("END DEBUG SET\n");
	}
#endif

}