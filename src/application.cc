#include <functional>

//#define DEBUG
//#define VERBOSE 0

#include <imgui.h>
#include <imgui-SFML.h>

#include "application.h"
#include "shape/polygon.h"
#include "shape/layer.h"
#include "utils/dialog.h"
#include "utils/svg.h"
#include "utils/color.h"

Application::Application(int window_width, int window_height, const sf::String& title) :
	window_main(std::make_unique<sf::RenderWindow>(sf::VideoMode(window_width, window_height), title)),
	window_width(window_width),
	window_height(window_height),
	layer_counter(0),
	state_manager(),
	picked_color_primary{ 0, 0, 0 },
	picked_color_secondary{ 0, 0, 0 },
	selected_layer_idx(-1),
	selected_fill_color_choice(0),
	current_polygon_buffer(nullptr),
	vertex_buffer(nullptr),
	mouse_hold(false)
{
	// Should be limited to avoid high computation
	// Without this, my computer will works really hard
	window_main->setFramerateLimit(60);

	ImGui::SFML::Init(*this->window_main);

	// Disable saving the ImGui configuration
	ImGui::GetIO().IniFilename = nullptr;
}

Application::~Application()
{
	ImGui::SFML::Shutdown();
}

void Application::update()
{
	for (Layer layer : layers)
	{
		layer.draw(window_main.get());
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
				if (filename != nullptr)
				{
					for (auto layer : layers)
					{
						layer.free();
					}
					layers.clear();
					utils::SVG::loadFromFile(filename, &layers);
				}
			}

			// Saving
			if (ImGui::MenuItem("Save"))
			{
				char const* filename = utils::Dialog::save();
				if (filename != nullptr)
				{
					utils::SVG svg(window_width, window_height, filename);
					svg.from(layers);
					svg.save();
				}
			}

			if (ImGui::MenuItem("Close"))
			{
				window_main->close();
			}

			ImGui::EndMenu();
		}
		auto mouse_pos = sf::Mouse::getPosition(*window_main);
		ImGui::Text("x: %d, y: %d", (int)mouse_pos.x, (int)mouse_pos.y);
#ifdef DEBUG
		ImGui::Text("State: %d", (int)state_manager.get());
#endif
	}
	ImGui::EndMainMenuBar();

	// Toolbox
	ImGui::SetNextWindowSize(ImVec2(200, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Polygon button
			if (ImGui::ImageButton(assets.icon.polygon, sf::Vector2f(30, 30)))
			{
				selected_layer_idx = -1;
				state_manager.set(State::DrawPolygon);

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

				state_manager.setLifeCycleEnd(State::DrawPolygon, [this]()
				{
					if (current_polygon_buffer->size() < 3)
					{
						layers.pop_back();
						delete current_polygon_buffer;
						current_polygon_buffer = nullptr;
						return;
					}
					current_polygon_buffer->endVertex();
					current_polygon_buffer = nullptr;
				});
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Polygon shape");
			}
		}

		if (ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorEdit3("Primary", picked_color_primary);
			ImGui::ColorEdit3("Secondary", picked_color_secondary);

			const char* fill_color_choice_items[] = { "No fill", "Primary color" };
			ImGui::Combo(
				"Fill",
				&selected_fill_color_choice,
				fill_color_choice_items,
				IM_ARRAYSIZE(fill_color_choice_items)
			);
		}

		// Bruh unsafe moment
		if (selected_layer_idx != -1)
		{
			layers.at(selected_layer_idx).object.polygon->setFillColor(picked_color_primary);
			layers.at(selected_layer_idx).object.polygon->setOutlineColor(picked_color_secondary);
			layers.at(selected_layer_idx).object.polygon->isFilled(selected_fill_color_choice);
		}

		if (ImGui::CollapsingHeader("Layer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BeginChild("", ImVec2(0, 200), true);

			int current_layer_idx = 0;
			for (auto layer = layers.begin(); layer != layers.end(); ++layer, ++current_layer_idx)
			{
				if (ImGui::Selectable(layer->name.c_str(), selected_layer_idx == current_layer_idx))
				{
					if (selected_layer_idx != -1)
					{
						state_manager.set(State::Nothing);
						layers[selected_layer_idx].object.polygon->isEditMode(false);
						current_polygon_buffer = nullptr;
					}

					selected_layer_idx = current_layer_idx;
					// Bruh unsafe moment
					selected_fill_color_choice = layers.at(current_layer_idx).object.polygon->isFilled();

					picked_color_primary[0] = layers.at(current_layer_idx).object.polygon->getFillColor().r / 255.0;
					picked_color_primary[1] = layers.at(current_layer_idx).object.polygon->getFillColor().g / 255.0;
					picked_color_primary[2] = layers.at(current_layer_idx).object.polygon->getFillColor().b / 255.0;

					picked_color_secondary[0] =
						layers.at(current_layer_idx).object.polygon->getOutlineColor().r / 255.0;
					picked_color_secondary[1] =
						layers.at(current_layer_idx).object.polygon->getOutlineColor().g / 255.0;
					picked_color_secondary[2] =
						layers.at(current_layer_idx).object.polygon->getOutlineColor().b / 255.0;
				}
			}
//				printf("selected_layer_idx %d\ncurrent_layer_idx %d\n", selected_layer_idx, current_layer_idx);
			ImGui::EndChild();

			if (selected_layer_idx != -1)
			{
				if (ImGui::Button("Deselect"))
				{
					state_manager.set(State::Nothing);

					selected_layer_idx = -1;
					current_polygon_buffer = nullptr;
				}

				// If not the last/front layer
				if (selected_layer_idx != layers.size() - 1)
				{
					if (ImGui::Button("Bring Front"))
					{
						state_manager.set(State::Nothing);

						Layer temp_layer(layers[selected_layer_idx]);
						layers[selected_layer_idx] = layers[selected_layer_idx + 1];
						layers[selected_layer_idx + 1] = temp_layer;
						selected_layer_idx += 1;
					}
				}

				// If not the first/back layer
				if (selected_layer_idx != 0)
				{
					if (ImGui::Button("Bring Back"))
					{
						state_manager.set(State::Nothing);

						Layer temp_layer(layers[selected_layer_idx]);
						layers[selected_layer_idx] = layers[selected_layer_idx - 1];
						layers[selected_layer_idx - 1] = temp_layer;
						selected_layer_idx -= 1;
					}
				}

				if (!state_manager.is(State::EditVertexPolygon))
				{
					if (ImGui::Button("Edit polygon vertex"))
					{
						state_manager.set(State::EditVertexPolygon);
						state_manager.setLifeCycleEnd(State::EditVertexPolygon, [this]()
						{
							layers[selected_layer_idx].object.polygon->isEditMode(false);
							current_polygon_buffer = nullptr;
						});

						layers[selected_layer_idx].object.polygon->isEditMode(true);
						current_polygon_buffer = layers[selected_layer_idx].object.polygon;
					}
				}
				else
				{
					if (ImGui::Button("Finish edit polygon vertex"))
					{
						state_manager.set(State::Nothing);
					}
				}

				if (selected_layer_idx != -1 && layers[selected_layer_idx].object.polygon->size() > 3)
				{
					if (!state_manager.is(State::DeleteVertexPolygon))
					{
						if (ImGui::Button("Delete polygon vertex"))
						{
							state_manager.set(State::DeleteVertexPolygon);
							state_manager.setLifeCycleEnd(State::DeleteVertexPolygon, [this]()
							{
								printf("END!\n");
								layers[selected_layer_idx].object.polygon->isEditMode(false);
								current_polygon_buffer = nullptr;
							});

							layers[selected_layer_idx].object.polygon->isEditMode(true);
							current_polygon_buffer = layers[selected_layer_idx].object.polygon;
						}
					}
					else
					{
						if (ImGui::Button("Finish delete polygon vertex"))
						{
							state_manager.set(State::Nothing);
						}
					}
				}

				if (ImGui::Button("Delete Selected Layer"))
				{
					state_manager.set(State::Nothing);

#ifdef DEBUG
					for (auto v : layers.at(selected_layer_idx).object.polygon->data())
					{
						printf("(%d, %d), ", (int)v.position.x, (int)v.position.y);
					}
					printf("\n");
#endif
					try
					{
						layers.at(selected_layer_idx).free();
						layers.erase(layers.begin() + selected_layer_idx);
					}
					catch (std::exception& ex)
					{
						printf("Error when deleting layer\n");
					}

					selected_layer_idx = -1;
				}
			}
		}
	}
	ImGui::End();
}

void Application::drawPolygonEvent(sf::Event& event)
{
	if (event_manager.isDoubleClick())
	{
		state_manager.set(State::Nothing);
	}
	else if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			state_manager.set(State::Nothing);
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
		event_manager.reset();
		while (window_main->pollEvent(event))
		{
			event_manager.processEvent(event);

			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window_main->close();
			}

			if (state_manager.is(State::DrawPolygon))
			{
				drawPolygonEvent(event);
			}
			else if (state_manager.is(State::EditVertexPolygon))
			{
				editVertexPolygonEvent(event);
			}
			else if (state_manager.is(State::DeleteVertexPolygon))
			{
				deleteVertexPolygonEvent(event);
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
		current_polygon_buffer->endVertex();
	}
}

void Application::deleteVertexPolygonEvent(sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			auto mouse_pos = sf::Mouse::getPosition(*window_main);
			current_polygon_buffer->removeNearestVertex(mouse_pos);
			current_polygon_buffer->endVertex();

			if (current_polygon_buffer->size() <= 3)
			{
				state_manager.set(State::Nothing);
				current_polygon_buffer = nullptr;
			}
		}
	}
}
