#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/OpenGL.hpp>

#include "application.h"
#include "shape/polygon.cc"

// debug purpose
sf::VertexBuffer polygonnn(sf::LineStrip);

Application::Application(int width, int height, const sf::String& title)
{
	window_main = new sf::RenderWindow(sf::VideoMode(width, height), title);
	state = State::DrawPolygon;
	this->width = width;
	this->height = height;

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

	for (sf::Drawable* shape : shape_layer)
	{
		window_main->draw(*shape);
	}
}

void Application::updateInterface()
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
			}
			if (ImGui::MenuItem("Close"))
			{
				window_main->close();
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (ImGui::Begin("Toolbox", nullptr,ImGuiWindowFlags_None))
	{
		if (ImGui::BeginTabBar(""))
		{
			if (ImGui::BeginTabItem("Shape"))
			{
				// Buttons here

			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void Application::drawPolygon(sf::Event &event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			if (!polygon_buffer.empty())
			{
				polygon_buffer.pop_back();
			}
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			sf::Vector2i flo = sf::Mouse::getPosition(*window_main);
			sf::Vertex mouse_click_position(sf::Vector2f(flo.x, flo.y), sf::Color::Black);
			polygon_buffer.push_back(mouse_click_position);

			printf("size=%zu\n", polygon_buffer.size());
		}
	}
}

void Application::dispatch()
{
	sf::Event event{};
	sf::Clock delta_clock;

	window_main->clear(sf::Color::White);

	// Sample
	shape::Polygon t;
	t.appendVertex(sf::Vertex(sf::Vector2f(200, 200), sf::Color::Black));
	t.appendVertex(sf::Vertex(sf::Vector2f(200, 150), sf::Color::Black));
	t.appendVertex(sf::Vertex(sf::Vector2f(120, 200), sf::Color::Black));
	t.appendVertex(sf::Vertex(sf::Vector2f(200, 200), sf::Color::Black));
	auto set = t.constructSortedEdgeTable();
	shape::Polygon::printSortedEdgeTable(set);
	shape_layer.push_back(&t);

	// debug purpose
	shape_layer.push_back(&polygonnn);

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
				drawPolygon(event);
			}
		}

		update();

		ImGui::SFML::Update(*window_main, delta_clock.restart());
		updateInterface();

		ImGui::SFML::Render(*window_main);

		window_main->draw(polygon_buffer.data(), polygon_buffer.size(), sf::LineStrip);
		window_main->display();
	}
}
