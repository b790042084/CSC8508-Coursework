/*		 Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Pickup Object Definition		 */
#pragma once
#include "GameObject.h"
#include "PlayerObject.h"
#include "EnemyStateGameObject.h"
namespace NCL {
	namespace CSC8503 {
		class PickupObject : public GameObject {
		public:
			PickupObject() {
				name = "Pickup";
				invMass = 0.0f;		// We don't have mass
				elasticity = 0.0;
				friction = 0.8;
			}
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(invMass);
				physicsObject->SetElasticity(elasticity);
				physicsObject->SetFriction(friction);
			}
		protected:
			float invMass;
			float elasticity;
			float friction;
		};
	}
}

