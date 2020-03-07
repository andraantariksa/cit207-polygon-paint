#define DEBUG
//#define VERBOSE 0

#include <imgui.h>
#include <imgui-SFML.h>

#include "application.h"
#include "shape/layer.h"
#include "utils/dialog.h"
#include "utils/svg.h"
#include "shape/polygon.h"

Application::Application(int width, int height, const sf::String& title) :
	window_main(new sf::RenderWindow(sf::VideoMode(width, height), title)),
	width(width),
	height(height),
	state(State::Nothing)
{
	ImGui::SFML::Init(*this->window_main);
	// Disable saving the ImGui configuration
	ImGui::GetIO().IniFilename = nullptr;
}

Application::~Application()
{
	ImGui::SFML::Shutdown();

	delete this->window_main;
}

void Application::update()
{
	for (Layer layer : layers)
	{
		layer.draw(window_main);
	}
}

void Application::updateInterface(Assets& assets)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
			}

			// Saving
			if (ImGui::MenuItem("Save"))
			{
				char const* filename = utils::Dialog::save();
				utils::SVG svg(width, height, filename);
				svg.from(layers);
				svg.save();
			}

			if (ImGui::MenuItem("Close"))
			{
				window_main->close();
			}

			ImGui::EndMenu();
		}
#ifdef DEBUG
		ImGui::Text("State: %d", (int) state);
#endif
	}
	ImGui::EndMainMenuBar();

	// Toolbox
	ImGui::SetNextWindowSize(ImVec2(200, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_None))
	{
//		if (ImGui::BeginTabBar(""))
//		{
			if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// Polygon Button
				if (ImGui::ImageButton(assets.icon.polygon, sf::Vector2f(30, 30)))
				{
					current_polygon_buffer = new shape::Polygon();

					Layer temp_layer;
					temp_layer.type = LayerObjectType::Polygon;
					temp_layer.object.polygon = current_polygon_buffer;
					temp_layer.name = "Newly created layer";

					layers.push_back(temp_layer);

					state = State::DrawPolygon;
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Polygon shape");
				}
			}

			if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::ColorPicker4("Color", picked_color);
			}

			if (ImGui::CollapsingHeader("Layer", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::BeginChild("", ImVec2(0, 200), true);
				int current_layer_idx = 0;
				for (auto layer = layers.begin(); layer != layers.end(); ++layer, ++current_layer_idx)
				{
					if (ImGui::Selectable(layer->name.c_str(), selected_layer_idx == current_layer_idx))
					{
						selected_layer_idx = current_layer_idx;
					}
				}
				ImGui::EndChild();

				if (ImGui::Button("Bring Front"))
				{
					if (selected_layer_idx != layers.size() - 1)
					{
						Layer temp_layer(layers[selected_layer_idx]);
						layers[selected_layer_idx] = layers[selected_layer_idx + 1];
						layers[selected_layer_idx + 1] = temp_layer;
						selected_layer_idx += 1;
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Bring Back"))
				{
					if (selected_layer_idx != 0)
					{
						Layer temp_layer(layers[selected_layer_idx]);
						layers[selected_layer_idx] = layers[selected_layer_idx - 1];
						layers[selected_layer_idx - 1] = temp_layer;
						selected_layer_idx -= 1;
					}
				}

				if (ImGui::Button("Delete Selected Layer"))
				{
					layers.at(selected_layer_idx).free();
					layers.erase(layers.begin() + selected_layer_idx);
				}
			}
//		}
//		ImGui::EndTabBar();

		ImGui::End();
	}
}

void Application::endDrawPolygonEvent()
{
	state = State::Nothing;

	current_polygon_buffer->endVertex();
	current_polygon_buffer = nullptr;
}

void Application::drawPolygonEvent(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			endDrawPolygonEvent();
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			sf::Vector2i flo = sf::Mouse::getPosition(*window_main);
			sf::Vertex mouse_click_position(sf::Vector2f(flo.x, flo.y), sf::Color::Black);
			current_polygon_buffer->appendVertex(mouse_click_position);

#ifdef DEBUG
			printf("Polygon buffer size=%d\n", current_polygon_buffer->size());
#endif
		}
	}
}

void Application::dispatch()
{
	sf::Event event{};
	sf::Clock delta_clock;
	Assets assets{};
	assets.icon.polygon.loadFromFile("../assets/icon/button-polygon.png");

	window_main->clear(sf::Color::White);

#ifdef DEBUG
	// Sample
	auto* t = new shape::Polygon();
	t->appendVertex(sf::Vertex(sf::Vector2f(200, 180), sf::Color::Black));
	t->appendVertex(sf::Vertex(sf::Vector2f(200, 150), sf::Color::Black));
	t->appendVertex(sf::Vertex(sf::Vector2f(280, 200), sf::Color::Black));
	t->endVertex();
	auto set = t->constructSortedEdgeTable();
	shape::Polygon::printSortedEdgeTable(set);

	Layer temp_layer;
	temp_layer.type = LayerObjectType::Polygon;
	temp_layer.object.polygon = t;
	temp_layer.name = "Bruh";

	layers.push_back(temp_layer);

	// Sample
	auto* t2 = new shape::Polygon();
	t2->appendVertex(sf::Vertex(sf::Vector2f(500, 180), sf::Color::Black));
	t2->appendVertex(sf::Vertex(sf::Vector2f(500, 150), sf::Color::Black));
	t2->appendVertex(sf::Vertex(sf::Vector2f(580, 300), sf::Color::Black));
	t2->endVertex();
	auto set2 = t2->constructSortedEdgeTable();
	shape::Polygon::printSortedEdgeTable(set2);

	temp_layer.type = LayerObjectType::Polygon;
	temp_layer.object.polygon = t2;
	temp_layer.name = "Moment";

	layers.push_back(temp_layer);

	auto* t3 = new shape::Polygon();
	t3->appendVertex(sf::Vertex(sf::Vector2f(500, 350), sf::Color::Black));
	t3->appendVertex(sf::Vertex(sf::Vector2f(500, 550), sf::Color::Black));
	t3->appendVertex(sf::Vertex(sf::Vector2f(600, 450), sf::Color::Black));
	t3->appendVertex(sf::Vertex(sf::Vector2f(700, 550), sf::Color::Black));
	t3->appendVertex(sf::Vertex(sf::Vector2f(700, 350), sf::Color::Black));
	t3->appendVertex(sf::Vertex(sf::Vector2f(600, 450), sf::Color::Black));
	t3->endVertex();
	auto set3 = t3->constructSortedEdgeTable();
	shape::Polygon::printSortedEdgeTable(set3);

	temp_layer.type = LayerObjectType::Polygon;
	temp_layer.object.polygon = t3;
	temp_layer.name = "Bruh2";

	layers.push_back(temp_layer);
#endif

	while (window_main->isOpen())
	{
		while (window_main->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window_main->close();
			}

			if (state == State::DrawPolygon)
			{
				drawPolygonEvent(event);
			}
		}

		window_main->clear(sf::Color::White);

		update();

#ifdef DEBUG
		t->fill(set, window_main);
		t2->fill(set2, window_main);
		t3->fill(set3, window_main);
#endif

		ImGui::SFML::Update(*window_main, delta_clock.restart());
		updateInterface(assets);

		ImGui::SFML::Render(*window_main);

		window_main->display();
	}
}
