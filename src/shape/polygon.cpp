//#define DEBUG
//#define VERBOSE 0

#include <algorithm>

#include "polygon.h"
#include "../utils/color.h"
#include "edge-bucket.h"

namespace shape
{
	Polygon::Polygon()
		:
		color_outline(sf::Color::Black),
		color_fill(sf::Color::Transparent),
		is_filled(false),
		finish(false),
		edit_mode(false)
	{
	}

	Polygon::Polygon(Polygon const& another_polygon)
		:
		vertices(another_polygon.vertices),
		color_outline(another_polygon.color_outline),
		color_fill(another_polygon.color_fill),
		is_filled(another_polygon.is_filled),
		edit_mode(false)
	{
	}

	Polygon::Polygon(float picked_color_primary[3], float picked_color_secondary[3], bool is_filled)
		:
		color_outline(color_float3(picked_color_secondary)),
		color_fill(color_float3(picked_color_primary)),
		finish(false),
		edit_mode(false),
		is_filled(is_filled)
	{
	}

	void Polygon::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		if (!sorted_edge_table.lines.empty() && is_filled)
		{
			fill(target);
		}

		auto rect = sf::RectangleShape(sf::Vector2f(vertex_edit_drag_max_distance, vertex_edit_drag_max_distance));
		rect.setOrigin((float)vertex_edit_drag_max_distance / 2, (float)vertex_edit_drag_max_distance / 2);
		rect.setFillColor(vertex_edit_color);

		if (edit_mode)
		{
			for (const auto& vertex : vertices)
			{
				rect.setPosition(vertex.position);
				target.draw(rect);
			}
		}

		target.draw(vertices.data(), vertices.size(), sf::LineStrip);

		if (finish)
		{
			sf::Vertex end_line[2] = { *--vertices.end(), *vertices.begin() };
			target.draw(end_line, 2, sf::LineStrip);
		}
	}

	void Polygon::appendVertex(sf::Vertex vertex)
	{
		vertices.push_back(vertex);
	}

	sf::Vertex Polygon::popBackVertex()
	{
		sf::Vertex ret = vertices.back();
		vertices.pop_back();
		return ret;
	}

	void Polygon::endVertex()
	{
		finish = true;

		// Should be connected, to generate the sorted edge table
		vertices.push_back(*vertices.begin());
		auto t = constructSortedEdgeTable();
		vertices.pop_back();
#ifdef DEBUG
		printSortedEdgeTable(t);
#endif
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
		auto [vertex_with_y_min, vertex_with_y_max] = std::minmax_element(vertices.begin(), vertices.end(), [](const sf::Vertex a, const sf::Vertex b){
			return a.position.y < b.position.y;
		});

		int table_size = (int) vertex_with_y_max->position.y - (int) vertex_with_y_min->position.y + 1;

#ifdef DEBUG
		printf("The sorted edge table size is %d - %d + 1 = %d\n", (int) vertex_with_y_max->position.y, (int) vertex_with_y_min->position.y, table_size);
#endif

		Polygon::SortedEdgeTable sorted_edge_table_temp = Polygon::SortedEdgeTable();
		sorted_edge_table_temp.lines.resize(table_size);
		sorted_edge_table_temp.y_min = (int) vertex_with_y_min->position.y;

		// For loop from beginning to end - 1, and taking 2 element each
		// [1, 2], [2, 3], ...
		for (auto current_vertex = vertices.begin(); current_vertex != std::prev(vertices.end()); ++current_vertex)
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

			if (sorted_edge_table_temp.lines[y_min - (int) vertex_with_y_min->position.y] == nullptr)
			{
				sorted_edge_table_temp.lines[y_min - (int) vertex_with_y_min->position.y] = new std::vector<EdgeBucket>();
			}

			sorted_edge_table_temp.lines[y_min - (int) vertex_with_y_min->position.y]->push_back(temp);
		}

		sorted_edge_table = sorted_edge_table_temp;

		return sorted_edge_table_temp;
	}

	void Polygon::fill(sf::RenderTarget &window) const
	{
#ifdef DEBUG
#if VERBOSE == 2
		printf("Fill Polygon\n");
#endif
#endif
		std::vector<EdgeBucket> active_edge_list;

		int i = -1;
		for (std::vector<EdgeBucket>* edge_buckets : sorted_edge_table.lines)
		{
			i++;

#ifdef DEBUG
#if VERBOSE == 2
			printf("Start active edge list for %d\n", i);
			for (auto edge_bucket : active_edge_list)
			{
				printf("  -> y_max=%d, x_of_y_min=%d, dx=%d, dy=%d, carry=%d\n", edge_bucket.y_max, edge_bucket.x_of_y_min, edge_bucket.dx, edge_bucket.dy, edge_bucket.carry);
			}
			printf("End active edge list\n");
#endif
#endif

			// I think we only have to sort on inserting the active edge list
			std::sort(active_edge_list.begin(), active_edge_list.end(), [](EdgeBucket a, EdgeBucket b)
			{
				return a.x_of_y_min < b.x_of_y_min;
			});

#ifdef DEBUG
#if VERBOSE == 2
			for (auto edge_bucket : active_edge_list)
			{
				sf::VertexArray l(sf::Points);
				l.append(sf::Vertex(sf::Vector2f(edge_bucket.x_of_y_min, sorted_edge_table.y_min + i), sf::Color::Red));
				window->draw(l);
			}
#endif
#endif

			for (auto edge_bucket = active_edge_list.begin(); edge_bucket != active_edge_list.end(); ++(++edge_bucket))
			{
				auto next_edge_bucket = std::next(edge_bucket);
				sf::VertexArray l(sf::Lines);
				l.append(sf::Vertex(sf::Vector2f(edge_bucket->x_of_y_min,
					sorted_edge_table.y_min + i), color_fill));
				l.append(sf::Vertex(sf::Vector2f(next_edge_bucket->x_of_y_min,
					sorted_edge_table.y_min + i), color_fill));
				window.draw(l);
			}

			active_edge_list.erase(
				std::remove_if(active_edge_list.begin(), active_edge_list.end(), [i](EdgeBucket &edge_bucket)
				{
					return !edge_bucket.next(i);
				}),
				active_edge_list.end()
			);

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
#ifdef DEBUG
#if VERBOSE == 2
		printf("End Fill Polygon\n");
#endif
#endif
	}

	int Polygon::size()
	{
		return vertices.size();
	}

	const std::vector<sf::Vertex>& Polygon::data()
	{
		return vertices;
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
				printf("-\n");
//				printf("  -> Edge ymax=%d, x_of_y_min=%d, dx=%d, dy=%d, carry=%d\n", edge_bucket.y_max, edge_bucket.x_of_y_min, edge_bucket.dx, edge_bucket.dy, edge_bucket.carry);
				printf("ymax = %d\nx_of_y_min = %d\ndx = %d\ndy = %d\ncarry = %d\n", edge_bucket.y_max, edge_bucket.x_of_y_min, edge_bucket.dx, edge_bucket.dy, edge_bucket.carry);
			}
			printf("\n");
		}
		printf("END DEBUG SET\n");
	}
#endif

	const sf::Color& Polygon::getFillColor()
	{
		return color_fill;
	}

	const sf::Color& Polygon::getOutlineColor()
	{
		return color_outline;
	}

	const bool& Polygon::isFilled()
	{
		return is_filled;
	}

	void Polygon::isEditMode(bool is_it)
	{
		edit_mode = is_it;
	}

	sf::Vertex* Polygon::getNearestVertex(sf::Vector2i pos)
	{
		for (auto& vertex : vertices)
		{
			if (
				std::abs((int)vertex.position.x - pos.x) <= vertex_edit_drag_max_distance &&
				std::abs((int)vertex.position.y - pos.y) <= vertex_edit_drag_max_distance)
			{
				return &vertex;
			}
		}
		return nullptr;
	}

	void Polygon::removeNearestVertex(sf::Vector2i pos)
	{
		int i = 0;
		for (auto& vertex : vertices)
		{
			if (
				std::abs((int)vertex.position.x - pos.x) <= vertex_edit_drag_max_distance &&
				std::abs((int)vertex.position.y - pos.y) <= vertex_edit_drag_max_distance)
			{
				vertices.erase(vertices.begin() + i);
			}

			i++;
		}
	}

	void Polygon::isFilled(bool is_it)
	{
		is_filled = is_it;
	}

	void Polygon::setFillColor(const float color[3])
	{
		color_fill.r = color[0] * 255.0;
		color_fill.g = color[1] * 255.0;
		color_fill.b = color[2] * 255.0;
	}

	void Polygon::setOutlineColor(const float color[3])
	{
		color_outline.r = color[0] * 255.0;
		color_outline.g = color[1] * 255.0;
		color_outline.b = color[2] * 255.0;

		for (auto& vertex: vertices)
		{
			vertex.color = color_outline;
		}
	}

	void Polygon::setFillColor(sf::Color& color)
	{
		color_fill = color;
	}

	void Polygon::setOutlineColor(sf::Color& color)
	{
		color_outline = color;
	}

}