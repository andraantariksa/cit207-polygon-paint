#include <SFML/Graphics/Drawable.hpp>
#include <cstring>

#include "tinyxml2.h"
#include "simple_svg.hpp"

#include "svg.h"
#include "../shape/polygon.h"
#include "../shape/layer.h"

namespace utils
{

	SVG::SVG(int width, int height, char const* filename)
		:
		dimension(width, height),
		document(filename, svg::Layout(dimension, svg::Layout::TopLeft))
	{
	}

	void SVG::from(std::vector<Layer>& layers)
	{
		// TODO
		// Make the shape become general, not only polygon
		svg::Point point;
		for (auto layer : layers)
		{
			if (layer.type == LayerObjectType::Polygon)
			{
				sf::Color sfmlPolygonFillColor = layer.object.polygon->getFillColor();
				svg::Color simpleSVGPolygonFillColor(
					(int) (sfmlPolygonFillColor.r),
					(int) (sfmlPolygonFillColor.g),
					(int) (sfmlPolygonFillColor.b)
				);

				sf::Color sfmlPolygonOutlineColor = layer.object.polygon->getOutlineColor();
				svg::Color simpleSVGPolygonOutlineColor(
					(int) (sfmlPolygonOutlineColor.r),
					(int) (sfmlPolygonOutlineColor.g),
					(int) (sfmlPolygonOutlineColor.b)
				);

				svg::Fill fillColor;

				if (layer.object.polygon->isFilled())
				{
					fillColor = svg::Fill(simpleSVGPolygonFillColor);
				}

				svg::Polygon polygon(fillColor, svg::Stroke(1, simpleSVGPolygonOutlineColor));
				for (auto vertex : ((shape::Polygon*) layer.object.polygon)->data())
				{
					point.x = vertex.position.x;
					point.y = vertex.position.y;
					polygon << point;
				}

				document << polygon;
			}
			else
			{
				throw std::logic_error("Not implemented");
			}
		}
	}

	void SVG::save()
	{
		document.save();
	}

	void SVG::loadFromFile(char const* filename, std::vector<Layer>* layers)
	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename);

		tinyxml2::XMLElement* xml_svg = doc.FirstChildElement("svg");

		int iter = 0;

		for(
			tinyxml2::XMLElement* xml_polygon = xml_svg->FirstChildElement("polygon");
			xml_polygon != nullptr;
			xml_polygon = xml_polygon->NextSiblingElement("polygon"), iter++
		)
		{
			Layer temp_layer;
			temp_layer.type = LayerObjectType::Polygon;
			temp_layer.name = std::string("Layer") + std::to_string(iter);

			std::vector<sf::Vertex> temp_vertices;

			const char* stroke = xml_polygon->Attribute("stroke");

			float secondary_color[3];

			// , or )   , rgb(
			char __trash, ___trash[4];
//			std::stringstream ss_color(stroke);
//			ss_color >> ___trash >> secondary_color[0] >> __trash >> secondary_color[1] >> __trash >> secondary_color[2] >> __trash;

			std::sscanf(stroke, "rgb(%f,%f,%f)", &secondary_color[0], &secondary_color[1], &secondary_color[2]);

			const char* points = xml_polygon->Attribute("points");
			std::stringstream ss_points(points);

			int x, y;

			while(ss_points >> x >> __trash >> y)
			{
				temp_vertices.push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::Black));
			}

			const char* fill = xml_polygon->Attribute("fill");

			float primary_color[3];

//			ss_color.str(fill);
//			ss_color >> ___trash >> primary_color[0] >> __trash >> primary_color[1] >> __trash >> primary_color[2] >> __trash;
			std::sscanf(fill, "rgb(%f,%f,%f)", &primary_color[0], &primary_color[1], &primary_color[2]);
			primary_color[0] /= 255.0f;
			primary_color[1] /= 255.0f;
			primary_color[2] /= 255.0f;

			bool is_filled = true;

			if (std::strcmp(fill, "none") == 0)
			{
				memset(primary_color, 0, sizeof(float) * 3);
				is_filled = false;
			}

			temp_layer.object.polygon = new shape::Polygon(primary_color, secondary_color, is_filled);

			for (auto vertex : temp_vertices)
			{
				temp_layer.object.polygon->appendVertex(vertex);
			}

			if (is_filled)
			{
//				temp_layer.object.polygon->constructSortedEdgeTable();
				temp_layer.object.polygon->endVertex();
			}

			layers->push_back(temp_layer);
		}
	}

};
