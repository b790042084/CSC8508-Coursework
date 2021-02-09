/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Pushdown automata Pause Screen Definition	 */
#pragma once
using namespace NCL;
using namespace CSC8503;
class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
			return PushdownResult::Pop;		// Remain inactive unless the user presses [P]
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
	}
};

