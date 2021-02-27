/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *				  Main File			 */
#include "TutorialGame.h"
#include "../../Common/Window.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/IntroScreen.h"
using namespace NCL;
using namespace CSC8503;
/* The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead.

This time, we've added some extra functionality to the window class - we can
hide or show the */
void GamePushdownAutomata(Window* w);

int main() {
	Window* w = Window::CreateGameWindow("Fall Guys!", 1280, 720, false);
	if (!w->HasInitialised())
		return -1;

	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

	GamePushdownAutomata(w);
	Window::DestroyGameWindow();		// After we have exited the automata (we've quit) destroy the window
}

/* This method drives the entire game on a pushdown automata */
void GamePushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen);
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR))
			w->ShowConsole(true);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT))
			w->ShowConsole(false);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T))
			w->SetWindowPosition(0, 0);
		if (!machine.Update(dt))
			return;
	}
}