#include "application.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/OpenGL.hpp>

#include "shape/polygon.cc"
#include "utils/SVG.cc"

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
	window_main->clear(sf::Color::White);

	for (sf::Drawable* shape : shape_layers)
	{
		window_main->draw(*shape);
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
			if (ImGui::MenuItem("Save"))
			{
				utils::SVG test(width, height, "bruh.svg");
				test.from(shape_layers);
				test.save();
			}
			if (ImGui::MenuItem("Close"))
			{
				window_main->close();
			}
			ImGui::EndMenu();
		}
#ifdef DEBUG
		ImGui::Text("State: %d", state);
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
					shape_layers.push_back(current_polygon_buffer);

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
				for (auto layer = shape_layers.begin(); layer != shape_layers.end(); ++layer, ++current_layer_idx)
				{
					char label[128];
					sprintf(label, "Polygon %d", current_layer_idx);
					if (ImGui::Selectable(label, selected_layer_idx == current_layer_idx))
					{
						selected_layer_idx = current_layer_idx;
					}
				}
				ImGui::EndChild();

				if (ImGui::Button("Delete Selected Layer"))
				{
					delete shape_layers.at(selected_layer_idx);
					shape_layers.erase(shape_layers.begin() + selected_layer_idx);
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
	t->appendVertex(sf::Vertex(sf::Vector2f(120, 200), sf::Color::Black));
	t->endVertex();
	auto set = t->constructSortedEdgeTable();
	shape::Polygon::printSortedEdgeTable(set);
	shape_layers.push_back(t);
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

		update();

		ImGui::SFML::Update(*window_main, delta_clock.restart());
		updateInterface(assets);

		ImGui::SFML::Render(*window_main);

		window_main->display();
	}
}
