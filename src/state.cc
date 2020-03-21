#include "state.h"

StateManager::StateManager(State initial_state)
{
}

bool StateManager::is(State state)
{
	return current_state == state;
}

void StateManager::set(State state)
{
	switch (current_state)
	{
	case State::Nothing:
		current_state = state;
		break;
//	case State::DrawPolygon:
//
//		break;
	default:
		current_state = state;
		break;
	}
}

State StateManager::get()
{
	return current_state;
}
