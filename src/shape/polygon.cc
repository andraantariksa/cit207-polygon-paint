#include <algorithm>

#include "polygon.h"

namespace shape
{
	Polygon::Polygon()
	{
	}

	void Polygon::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(vertexes.data(), vertexes.size(), sf::LineStrip);
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

	void Polygon::endVertex()
	{
		vertexes.push_back(*vertexes.begin());
	}

	Polygon::SortedEdgeTable Polygon::constructSortedEdgeTable()
	{
		// Next vertex iterator
		std::vector<sf::Vertex>::iterator next_vertex;

		EdgeBucket temp{};
		temp.carry = 0; // Because the carry is always 0

		// Used for sorting the vertex by y value
		// the vertex with y-minimum would be the in the 1st element (Index 0)
		// and vice-versa
		sf::Vector2f sorted_by_y_vertex[2];

		// Get the size of table
		auto [vertex_with_y_min, vertex_with_y_max] = std::minmax_element(vertexes.begin(), vertexes.end(), [](const sf::Vertex a, const sf::Vertex b){
			return a.position.y < b.position.y;
		});
		int table_size = (int) vertex_with_y_max->position.y - (int) vertex_with_y_min->position.y + 1;

		Polygon::SortedEdgeTable sorted_edge_table = Polygon::SortedEdgeTable();
		sorted_edge_table.lines.resize(table_size);
		sorted_edge_table.y_min = (int) vertex_with_y_min->position.y;

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
				sorted_by_y_vertex + 2,
				[](const sf::Vector2f a, const sf::Vector2f b){
					return a.y < b.y;
			});

			// If it's a horizontal line
			if ((int) sorted_by_y_vertex[0].y == (int) sorted_by_y_vertex[1].y)
			{
				continue;
			}

			int y_min = (int) sorted_by_y_vertex[0].y;
//			printf("y_max=%d, y_min=%d\n", (int) sorted_by_y_vertex[0].y, (int) sorted_by_y_vertex[1].y);
			temp.y_max = (int) sorted_by_y_vertex[1].y - (int) vertex_with_y_min->position.y;
			temp.x_of_y_min = (int) sorted_by_y_vertex[0].x;

			// Delta
			temp.dx = (int) next_vertex->position.x - (int) current_vertex->position.x;
			temp.dy = (int) next_vertex->position.y - (int) current_vertex->position.y;

			if (temp.dy < 0)
			{
				temp.dy *= -1;
				temp.dx *= -1;
			}

//			printf("Lowest=%d, Vertex low=%d\n", (int) vertex_with_y_min->position.y, y_min);
			if (sorted_edge_table.lines[y_min - (int) vertex_with_y_min->position.y] == nullptr)
			{
				sorted_edge_table.lines[y_min - (int) vertex_with_y_min->position.y] = new std::vector<EdgeBucket>();
			}

			sorted_edge_table.lines[y_min - (int) vertex_with_y_min->position.y]->push_back(temp);
		}
		return sorted_edge_table;
	}

	void Polygon::fill(Polygon::SortedEdgeTable &sorted_edge_table, sf::RenderWindow* window)
	{
		std::vector<EdgeBucket> active_edge_list;

		// Iterate from bottom
		int i = -1;
		for (auto edge_buckets : sorted_edge_table.lines)
		{
			i++;
			// Check for expiring bucket
			for (auto edge_bucket : active_edge_list)
			{
				// Increment the y value
				if (!edge_bucket.next(i))
				{

				}
			}

			if (edge_buckets == nullptr)
			{
				continue;
			}

			// Add bucket
			for (auto edge_bucket : *edge_buckets)
			{
				active_edge_list.push_back(edge_bucket);
			}
		}
	}

	int Polygon::size()
	{
		return vertexes.size();
	}

	const std::vector<sf::Vertex>& Polygon::data()
	{
		return vertexes;
	}

#ifdef DEBUG
	void Polygon::printSortedEdgeTable(Polygon::SortedEdgeTable &sorted_edge_table)
	{
		printf("DEBUG SET\n");
		printf("SET size is %zu\n", sorted_edge_table.lines.size());

		int i = -1;
		for (std::vector<EdgeBucket>* edge_buckets : sorted_edge_table.lines)
		{
			i++;
			if (edge_buckets == nullptr)
			{
				continue;
			}
			printf("%d.\n", i);
			for (EdgeBucket edge_bucket : *edge_buckets)
			{
				printf("  -> Edge ymax=%d, x_of_y_min=%d, dx=%d, dy=%d, carry=%d\n", edge_bucket.y_max, edge_bucket.x_of_y_min, edge_bucket.dx, edge_bucket.dy, edge_bucket.carry);
			}
			printf("\n");
		}
		printf("END DEBUG SET\n");
	}
#endif

	bool EdgeBucket::next(int y_pos)
	{
		// Frac
		// dx    1
		// -- = ---
		// dy    m

		// TODO
		// WRONG
		if (dx > dy)
		{
			carry += dy;
			if (2 * carry >= dx)
			{
				carry -= dx;
				x_of_y_min++;
			}
		}
		else
		{
			carry += dx;
			if (2 * carry >= dy)
			{
				carry -= dy;
				x_of_y_min++;
			}
		}

		return y_pos < y_max;
	}
}