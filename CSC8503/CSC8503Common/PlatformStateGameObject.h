/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Platform State Game Object Definition	 */
#pragma once
#include "StateGameObject.h"
namespace NCL {
	namespace CSC8503 {
		class PlatformStateGameObject : public StateGameObject {
		public:
			PlatformStateGameObject(Vector3 start, Vector3 end);
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(0.0f);
				physicsObject->SetElasticity(0.2f);
				physicsObject->SetFriction(0.8f);
			}
			void MoveToLocation(float dt);
		protected:
			State* followRouteState;
			Vector3 speed;
			Vector3 start;
			Vector3 end;
			float invMass;
			float elasticity;
			float friction;
		};
	}
}
