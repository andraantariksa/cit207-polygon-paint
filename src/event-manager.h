#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

#include <SFML/Window.hpp>

class EventManager
{
public:
	EventManager();

	void processEvent(sf::Event& event);
	void reset();

	bool isDoubleClick();
private:
	bool is_double_click;
	sf::Clock clock;
	sf::Vector2i prev_mouse_pos;
};

#endif //_EVENT_MANAGER_H_
