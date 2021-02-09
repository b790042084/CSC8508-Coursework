/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Enemy Object Definition		 */
#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class EnemyObject : public GameObject {
		public:
			EnemyObject() {
				name = "Enemy";
				invMass = 5.0f;
				elasticity = 0.2;
				friction = 0.2;
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