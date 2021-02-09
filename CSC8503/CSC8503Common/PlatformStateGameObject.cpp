/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Platform State Game Object Implementation	 */
#include "PlatformStateGameObject.h"
using namespace NCL;
using namespace CSC8503;
PlatformStateGameObject::PlatformStateGameObject(Vector3 start, Vector3 end) {
	currentState = state::FOLLOWROUTE;
	speed = Vector3();
	invMass = 0.0f;
	elasticity = 0.0f;
	friction = 0.5f;
	this->start = start;
	this->end = end;
	name = "MovingFloor";
	followRouteState = new State([&](float dt)->void {
		this->MoveToLocation(dt);
		});
	stateMachine->AddState(followRouteState);
	stateMachine->AddTransition(new StateTransition(idleState, followRouteState, [&]()->bool {
		if ((this->start - this->end).Length() > 0.0f) {
			currentState == state::FOLLOWROUTE;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(followRouteState, idleState, [&]()->bool {
		if ((this->start - this->end).Length() == 0.0f) {
			currentState == state::IDLE;
			return true;
		}
		return false;
		}));
	name = "PlatformAI";
}

void PlatformStateGameObject::MoveToLocation(float dt) {
	/* Changes all directions based on on current location, done this way as we want linear velocity, not forces */
	if (abs(start.x - end.x) > 0.0) {
		if (abs(GetTransform().GetPosition().x - start.x) < 0.5)
			speed.x = (start.x - end.x > 0) ? -5 : 5;
		if (abs(GetTransform().GetPosition().x - end.x) < 0.5)
			speed.x = (start.x - end.x > 0) ? 5 : -5;
	}
	if (abs(start.y - end.y) > 0.0) {
		if (abs(GetTransform().GetPosition().y - start.y) < 0.5)
			speed.y = (start.y - end.y > 0) ? -5 : 5;
		if (abs(GetTransform().GetPosition().y - end.y) < 0.5)
			speed.y = (start.y - end.y) > 0 ? 5 : -5;
	}
	if (abs(start.z - end.z) > 0.0) {
		if (abs(GetTransform().GetPosition().z - start.z) < 0.5)
			speed.z = (start.z - end.z > 0) ? -5 : 5;
		if (abs(GetTransform().GetPosition().z - end.z) < 0.5)
			speed.z = (start.z - end.z > 0) ? 5 : -5;
	}
	GetPhysicsObject()->SetLinearVelocity(speed);
}