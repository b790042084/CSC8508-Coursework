/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Pushdown Automata IntroScreen Definition	 */
#pragma once
#include "PushdownState.h"
#include "../CSC8503Common/GlobalVariables.h"
#include "../GameTech/TutorialGame.h"
using namespace NCL;
using namespace CSC8503;
bool quit = false;		// Pull our global variable from the namespace 
class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (quit)
			return PushdownResult::Pop;		// Ends the program 
		else {
			*newState = new TutorialGame();		// Otherwise, create a new tutorial game and delete the old one
			return PushdownResult::Push;
		}
		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
	}
};

