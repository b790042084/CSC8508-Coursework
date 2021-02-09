/*		 Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Spring Object Definition		 */
#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class SpringObject : public GameObject {
		public:
			SpringObject(Vector3 rest, float springK) {
				invMass = 20.0f;		// Lighter than regular objects
				elasticity = 0.2;
				friction = 0.0;
				k = springK;
				name = "SpringCube";
				restPosition = rest;
			}
			void SetPhysicsObject(PhysicsObject* newObject) override {
				physicsObject = newObject;
				physicsObject->SetInverseMass(invMass);
				physicsObject->SetElasticity(elasticity);
				physicsObject->SetFriction(friction);
			}
			Vector3 GetRestPosition() const {
				return restPosition;
			}
			void SetRestPosition(Vector3 val) {
				restPosition = val;
			}
			float GetK() const {
				return k;
			}
		protected:
			float invMass;
			float elasticity;
			float friction;
			float k;
			Vector3 restPosition;
		};
	}
}