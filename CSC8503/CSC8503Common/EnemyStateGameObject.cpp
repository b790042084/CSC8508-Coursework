/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Enemy State Game Object Implementation	 */
#include "EnemyStateGameObject.h"
#include <algorithm>
#include "../../Common/Maths.h"
using namespace NCL;
using namespace CSC8503;
EnemyStateGameObject::EnemyStateGameObject() {
	invMass = 5;
	elasticity = 0.2;
	friction = 0.2;
	rayTime = 0.5f;
	finished = false;
	followTimeout = 0.0f;
	speed = 0.0f;
	displayPath = false;
	followObjectState = new State([&](float dt)->void {
		this->FollowObject(dt);
		if (displayPath)
			this->DisplayDirection();
		});
	stateMachine->AddState(followObjectState);
	stateMachine->AddTransition(new StateTransition(idleState, followObjectState, [&]()->bool {
		if (this->followTimeout > 0.0f) {
			currentState = state::FOLLOWOBJECT;
			return true;
		}
		return false;
		}));
	stateMachine->AddTransition(new StateTransition(followObjectState, idleState, [&]()->bool {
		if (this->followTimeout < 0.0f) {
			currentState = state::IDLE;
			return true;
		}
		return false;
		}));
	name = "EnemyAI";
}

void EnemyStateGameObject::Update(float dt) {
	/* Sort our objects of interest and follow the closest */
	for (auto itr = interestObjects.cbegin(); itr != interestObjects.cend();) {
		GameObject* o = *itr;
		Vector3 dir = o->GetTransform().GetPosition() - this->GetTransform().GetPosition();
		dir.y = 0;
		if (!currentObject)
			currentObject = o;
		else if (dir.Length() < travelDir.Length()) {		// If there is a closer object, prioritise it 
			currentObject = o;
			currentObject->SetTimeInSet(0.0f);
		}
		o->IncreaseTimeInSet(dt);
		/* Erase any objects that have existed too long, no longer exist, or are too far away */
		if (o->GetTimeInSet() > 10.0f || !o->IsActive() || dir.Length() > 80.0f) {
			itr = interestObjects.erase(itr);
			if (interestObjects.size() == 0) {
				followTimeout = 0.0f;
				break;
			}
		}
		else
			++itr;
	}
	rayTime -= dt;
	/* Set our speed if we have picked up a powerup */
	if (powerUpTimer > 0.0f) {
		speed = 5.0f * dt;
		powerUpTimer -= dt;
	}
	else {
		GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		speed = 2.5f * dt;
	}
	/* Manipulating rotation matrix to face current direction */
	Vector3 up = Vector3::Cross(Vector3(0, 1, 0), travelDir.Normalised());
	if (currentState != state::IDLE) {
		if (up.x >= 0.999 && (up.z <= 0.001 || (up.z >= -0.001 && up.z < 0)))
			GetTransform().SetOrientation(Matrix4::Rotation(180, Vector3(0, 1, 0)));
		else {
			Matrix3 rotMatrix;
			rotMatrix.SetColumn(0, -up.Normalised());
			rotMatrix.SetColumn(1, Vector3(0, 1, 0));
			rotMatrix.SetColumn(2, -travelDir.Normalised());
			GetTransform().SetOrientation(rotMatrix);
		}
		GetPhysicsObject()->ApplyLinearImpulse(Vector3(std::clamp(travelDir.x, -speed, speed), 0, std::clamp(travelDir.z, -speed, speed)));
		stateMachine->Update(dt);
	}
}

void EnemyStateGameObject::FollowObject(float dt) {
	followTimeout -= dt;
	travelDir = currentObject->GetTransform().GetPosition() - this->GetTransform().GetPosition();
	travelDir.y = 0;
}

void EnemyStateGameObject::DisplayDirection() {
	Debug::DrawLine(GetTransform().GetPosition(), currentObject->GetTransform().GetPosition(), Debug::CYAN);
}

