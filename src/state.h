#ifndef _STATE_H_
#define _STATE_H_

enum class State
{
	Nothing,
	DrawPolygon,
	EditVertexPolygon
};

class StateManager
{
public:
	StateManager(State initial_state);

	bool is(State state);
	void set(State state);
	State get();
private:
	State current_state;
};

#endif //_STATE_H_
