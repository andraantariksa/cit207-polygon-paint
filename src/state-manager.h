#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include <functional>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

enum class State
{
	Nothing,
	DrawPolygon,
	EditVertexPolygon,
	DeleteVertexPolygon
};

class StateManager
{
public:
	StateManager(State initial_state = State::Nothing);

	bool is(State state);
	void set(State state);
	State get();
	void setLifeCycleEnd(State state, std::function<void()> callback);
private:
	// Lifecycle
	std::unordered_map<State, std::function<void()>> lifecycle_end;

	State current_state;
};

#endif //_STATE_MANAGER_H_
