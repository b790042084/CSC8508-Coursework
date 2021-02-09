/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Behaviour Tree Enemy Implementation			 */
#include "BehaviourTreeEnemy.h"
using namespace NCL;
using namespace CSC8503;
BehaviourTreeEnemy::BehaviourTreeEnemy(Vector3 val) {
	BehaviourAction* idling = new BehaviourAction("Idling", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			GetPhysicsObject()->ClearForces();
			GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
			GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
			idleTimer = rand() % 3 + 2;		// How long will we wait until we do something 
			currentBehaviour = behaviour::IDLE;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			idleTimer -= dt;
			if (idleTimer <= 0.0f) 
				return Success;
		}
		return state;
		});

	BehaviourAction* moveToLocation = new BehaviourAction("Move to Random Location", [&](float dt, BehaviourState state)->BehaviourState {
		/* Choose a random location near us */
		if (state == Initialise) {
			randomLocation.x = rand() % 2 ? startPos.x + rand() % 20 : startPos.x - rand() % 20;
			randomLocation.z = rand() % 2 ? startPos.z + rand() % 20 : startPos.z - rand() % 20;
			currentBehaviour = behaviour::MOVING;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			travelDir = (randomLocation - GetTransform().GetPosition());
			travelDir.y = 0;
			if (travelDir.Length() <= 5.0f) {		// If we're close enough (y axis is ignored because we can't fly)
				GetPhysicsObject()->ClearForces();
				return Success;
			}
			else
				GetPhysicsObject()->ApplyLinearImpulse(Vector3(std::clamp(travelDir.x, -speed, speed), 0, std::clamp(travelDir.z, -speed, speed)));
		}
		return state;
		});

	BehaviourAction* searchLocation = new BehaviourAction("Look Around", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			turning = true;
			rayCast = true;
			searchTime = rand() % 10 + 5;		// How long do we spend scanning the environment
			halfTime = searchTime / 3;
			GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
			currentBehaviour = behaviour::SCANNING;
			return Ongoing;
		}
		else if (state == Ongoing) {
			searchTime -= dt;
			rayTime -= dt;
			if (searchTime < 0.0f || foundObject) {		// If we run out of time or find something 
				GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
				turning = false;
				rayCast = false;
				return Success;
			}
			else 
				searchTime > 2 * halfTime ? GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0.5f, 0)) :
					GetPhysicsObject()->SetAngularVelocity(Vector3(0, -0.5f, 0));
		}
		return state;
		});

	BehaviourAction* searchForObject = new BehaviourAction("Chase Object", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			if (foundObject) {
				currentBehaviour = behaviour::CHASING;
				return Ongoing;
			}
			else
				return Failure;
		}
		else if (state == Ongoing) {
			if(foundObject)
				travelDir = foundObject->GetTransform().GetPosition() - GetTransform().GetPosition();
			travelDir.y = 0;
			if (travelDir.Length() <= 3.0f)
				return Success;
			else if (travelDir.Length() > 50.0f)		// If object gets too far away, return 
				return Failure;
			else
				GetPhysicsObject()->ApplyLinearImpulse(Vector3(std::clamp(travelDir.x, -speed, speed), 0, std::clamp(travelDir.z, -speed, speed)));
		}
		return state;
		});

	BehaviourAction* returnToStart = new BehaviourAction("Return Home", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			searchTime = 10.0f;
			currentBehaviour = behaviour::RETURN;
			return Ongoing;
		}
		else if (state == Ongoing) {
			searchTime -= dt;
			travelDir = (startPos - GetTransform().GetPosition());
			travelDir.y = 0;
			if (travelDir.Length() <= 3.0f)
				return Success;
			else if (searchTime < 0.0f)
				return Failure;
			else
				GetPhysicsObject()->ApplyLinearImpulse(Vector3(std::clamp(travelDir.x, -speed, speed), 0, std::clamp(travelDir.z, -speed, speed)));
		}
		return state;
		});

	BehaviourSequence* sequence = new BehaviourSequence("Search Sequence");
	sequence->AddChild(idling);
	sequence->AddChild(moveToLocation);
	sequence->AddChild(searchLocation);

	BehaviourSelector* selection = new BehaviourSelector("Object Selection");
	selection->AddChild(searchForObject);
	selection->AddChild(returnToStart);

	rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	startPos = val;
	idleTimer = 0.0f;
	searchTime = 0.0f;
	halfTime = 0.0f;
	rayCast = false;
	rayTime = 0.0f;
	turning = false;
	reset = false;
	speed = 0.1f;
	foundObject = nullptr;
	displayPath = false;
	state = Ongoing;
	name = "BehaviourTreeEnemy";
}

void BehaviourTreeEnemy::Update(float dt) {
	/* If we pick up a power up, add more force */
	if (powerUpTimer > 0.0f) {
		speed = 25.0f * dt;
		powerUpTimer -= dt;
	}
	else {
		GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		speed = 12.5f * dt;
	}
	if (displayPath)
		DisplayRoute();
	if (state == Ongoing && !turning) {
		/* Rotation matrix manipulation to face the current direction */
		Vector3 up = Vector3::Cross(Vector3(0, 1, 0), travelDir.Normalised());
		if (up.x >= 0.995 && (up.z <= 0.005 || (up.z >= -0.005 && up.z < 0)))
			GetTransform().SetOrientation(Matrix4::Rotation(180, Vector3(0, 1, 0)));		// If we go directly backwards, just flip on the same axis
		else {
			Matrix3 rotMatrix;
			rotMatrix.SetColumn(0, -up.Normalised());
			rotMatrix.SetColumn(1, Vector3(0, 1, 0));
			rotMatrix.SetColumn(2, -travelDir.Normalised());
			GetTransform().SetOrientation(rotMatrix);
		}
	}
	if (state == Ongoing)
		state = rootSequence->Execute(dt);
	else if (state == Success || state == Failure || reset) {
		idleTimer = 0.0f;
		searchTime = 0.0f;
		halfTime = 0.0f;
		rayCast = false;
		rayTime = 0.0f;
		turning = false;
		reset = false;
		foundObject = nullptr;
		speed = 0.1f;
		state = Ongoing;
		rootSequence->Reset();
	}
}

string BehaviourTreeEnemy::BehaviourToString() const {
	switch (currentBehaviour) {
	case behaviour::IDLE:
		return "Idling";
	case behaviour::MOVING:
		return "Travelling to Area";
	case behaviour::SCANNING:
		return "Scanning Area";
	case behaviour::CHASING:
		return "Chasing Raycasted Object";
	case behaviour::RETURN:
		return "Returning Home";
	default:
		return "State Not Known";
	}
}

void BehaviourTreeEnemy::DisplayRoute() {
	switch (currentBehaviour) {
	case behaviour::MOVING:
		Debug::DrawLine(GetTransform().GetPosition(), randomLocation + Vector3(0, 3, 0), Debug::WHITE);
		break;
	case behaviour::CHASING:
		if(foundObject)
			Debug::DrawLine(GetTransform().GetPosition(), foundObject->GetTransform().GetPosition(), Debug::CYAN);
		break;
	case behaviour::RETURN:
		Debug::DrawLine(GetTransform().GetPosition(), startPos, Debug::WHITE);
		break;
	}
}