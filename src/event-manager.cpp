#include "event-manager.h"

EventManager::EventManager() :
	is_double_click(false)
{
}

bool EventManager::isDoubleClick()
{
	return is_double_click;
}

void EventManager::processEvent(sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
	{
		sf::Vector2i current_mouse_pos = sf::Mouse::getPosition();
		if (prev_mouse_pos == current_mouse_pos && clock.getElapsedTime().asMilliseconds() <= 300)
		{
			is_double_click = true;
		}
		else
		{
			prev_mouse_pos = current_mouse_pos;
			clock.restart();
		}
	}
}

void EventManager::reset()
{
	is_double_click = false;
}
