#include "state-manager.h"

StateManager::StateManager(State initial_state) :
	current_state(initial_state)
{
}

bool StateManager::is(State state)
{
	return current_state == state;
}

void StateManager::set(State state)
{
	try
	{
//		printf("called!\n");
		lifecycle_end.at(current_state)();
//		printf("called!2\n");
	}
	catch (std::out_of_range& ex)
	{
//		printf("Not found!\n");
	}
	catch (std::exception& ex)
	{
//		printf("Fail wtf idk\n");
	}

	current_state = state;
}

State StateManager::get()
{
	return current_state;
}

void StateManager::setLifeCycleEnd(State state, std::function<void()> callback)
{
	lifecycle_end.insert_or_assign(state, callback);
}
