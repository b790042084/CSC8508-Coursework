/*		Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game Object Definition		 */
#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class FloorObject : public GameObject {
		public:
			FloorObject() {
				invMass = 0.0f;		// No mass for floors
				elasticity = 0.2f;
				friction = 0.1f;
				name = "Floor";
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