#define DEBUG
//#define VERBOSE 0

#include <imgui.h>
#include <imgui-SFML.h>
#include <cstring>

#include "application.h"
#include "shape/layer.h"
#include "utils/dialog.h"
#include "utils/svg.h"
#include "shape/polygon.h"
#include "utils/color.h"

Application::Application(int width, int height, const sf::String& title) :
	window_main(new sf::RenderWindow(sf::VideoMode(width, height), title)),
	width(width),
	height(height),
	state(State::Nothing),
	layer_counter(0),
	selected_layer_idx(-1),
	selected_fill_color_choice(0),
	mouse_hold(false)
{
	window_main->setFramerateLimit(60);

	std::memset(picked_color_primary, 0, sizeof(float) * 3);
	std::memset(picked_color_secondary, 0, sizeof(float) * 3);

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
				char const* filename = utils::Dialog::load();
				for (auto layer : layers)
				{
					layer.free();
				}
				layers.clear();
				utils::SVG::loadFromFile(filename, &layers);
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
		auto mouse_pos = sf::Mouse::getPosition(*window_main);
		ImGui::Text("x: %d, y: %d", (int) mouse_pos.x, (int) mouse_pos.y);
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
					current_polygon_buffer = new shape::Polygon(
						picked_color_primary,
						picked_color_secondary,
						selected_fill_color_choice == 1
					);

					Layer temp_layer;
					temp_layer.type = LayerObjectType::Polygon;
					temp_layer.object.polygon = current_polygon_buffer;
					temp_layer.name = std::string("Layer ") + std::to_string(layer_counter++);

					layers.push_back(temp_layer);

					state = State::DrawPolygon;
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Polygon shape");
				}

//				ImGui::BeginCombo("Fill");
//				ImGui::Combo("No fill");
//				ImGui::Combo("Use primary color");
//				ImGui::EndCombo();
				const char* fill_color_choice_items[] = { "No fill", "Primary color" };
				ImGui::Combo(
					"Fill Color",
					&selected_fill_color_choice,
					fill_color_choice_items,
					IM_ARRAYSIZE(fill_color_choice_items)
				);
			}

			// TODO
			// Hollow and filled option

			if (ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::ColorEdit3("Primary Color", picked_color_primary);
				ImGui::ColorEdit3("Secondary Color", picked_color_secondary);
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
//				printf("selected_layer_idx %d\ncurrent_layer_idx %d\n", selected_layer_idx, current_layer_idx);
				ImGui::EndChild();

				if (ImGui::Button("Edit polygon"))
				{
					layers[selected_layer_idx].object.polygon->startEditMode();
					state = State::EditVertexPolygon;
					current_polygon_buffer = layers[selected_layer_idx].object.polygon;
				}

				if (ImGui::Button("Bring Front"))
				{
					if (!layers.empty() && selected_layer_idx != layers.size() - 1)
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
					if (!layers.empty() && selected_layer_idx != 0)
					{
						Layer temp_layer(layers[selected_layer_idx]);
						layers[selected_layer_idx] = layers[selected_layer_idx - 1];
						layers[selected_layer_idx - 1] = temp_layer;
						selected_layer_idx -= 1;
					}
				}

				if (ImGui::Button("Delete Selected Layer"))
				{
					if (!layers.empty())
					{
#ifdef DEBUG
						for (auto v : layers.at(selected_layer_idx).object.polygon->data())
						{
							printf("(%d, %d), ", (int) v.position.x, (int) v.position.y);
						}
						printf("\n");
#endif
						layers.at(selected_layer_idx).free();
						layers.erase(layers.begin() + selected_layer_idx);
					}
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
			sf::Vertex mouse_click_position(sf::Vector2f(flo.x, flo.y), color_float3(picked_color_secondary));
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

//	float r[3] = {1.0f, 0.0f, 0.0f};
//	float black[3] = {0.0f, 0.0f, 0.0f};
//	Layer l;
//	l.type = LayerObjectType::Polygon;
//	l.name = "Rectangle";
//	l.object.polygon = new shape::Polygon(r, black, true);
//	l.object.polygon->appendVertex(sf::Vertex(sf::Vector2f(300, 200), sf::Color::Black));
//	l.object.polygon->appendVertex(sf::Vertex(sf::Vector2f(400, 200), sf::Color::Black));
//	l.object.polygon->appendVertex(sf::Vertex(sf::Vector2f(450, 300), sf::Color::Black));
//	l.object.polygon->appendVertex(sf::Vertex(sf::Vector2f(250, 250), sf::Color::Black));
//	l.object.polygon->endVertex();
//
//	for (auto v : l.object.polygon->data())
//	{
//		printf("(%d, %d), ", (int) v.position.x, (int) v.position.y);
//	}
//	printf("\n");

//	layers.push_back(l);

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
			else if (state == State::EditVertexPolygon)
			{
				editVertexPolygonEvent(event);
			}
		}

		window_main->clear(sf::Color::White);

		update();

		ImGui::SFML::Update(*window_main, delta_clock.restart());
		updateInterface(assets);

		ImGui::SFML::Render(*window_main);

		window_main->display();
	}
}

void Application::editVertexPolygonEvent(sf::Event& event)
{
	auto mouse_pos = sf::Mouse::getPosition(*window_main);

	if (mouse_hold)
	{
		vertex_buffer->position.x = mouse_pos.x;
		vertex_buffer->position.y = mouse_pos.y;
	}

	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (!mouse_hold)
		{
			sf::Vertex* vertex = current_polygon_buffer->getNearestVertex(mouse_pos);
			if (vertex != nullptr)
			{
				vertex_buffer = vertex;
				mouse_hold = true;
			}
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		mouse_hold = false;
	}
}

void Application::endVertexPolygonEvent()
{

}
