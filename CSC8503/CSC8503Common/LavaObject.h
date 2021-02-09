/*		  Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Lava Object Definition		 */
#pragma once
#include "FloorObject.h"
#include "PlayerObject.h"
#include "PathFindingEnemyStateGameObject.h"
#include "PatrolEnemyStateGameObject.h"
#include "BehaviourTreeEnemy.h"

namespace NCL {
	namespace CSC8503 {
		class LavaObject : public FloorObject {
		public:
			LavaObject() {
				name = "Lava";
			}
			/* Whenever something hits lava, we need to be very careful about what happens */
			void OnCollisionBegin(GameObject* otherObject) override {
				otherObject->GetPhysicsObject()->ClearForces();
				otherObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
				otherObject->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
				if (dynamic_cast<PlayerObject*>(otherObject)) 
					((PlayerObject*)otherObject)->GetTransform().SetPosition(((PlayerObject*)otherObject)->GetSpawnPos());		// Set player to their checkpoint
				else if (dynamic_cast<EnemyStateGameObject*>(otherObject)) {
					((EnemyStateGameObject*)otherObject)->ClearFollowObjects();
					if (dynamic_cast<PathFindingEnemyStateGameObject*>(otherObject))
						((PathFindingEnemyStateGameObject*)otherObject)->GetTransform().
						SetPosition(((PathFindingEnemyStateGameObject*)otherObject)->GetPath()[0] + Vector3(0, 10, 0));
					if (dynamic_cast<PatrolEnemyStateGameObject*>(otherObject))
						((PatrolEnemyStateGameObject*)otherObject)->GetTransform().SetPosition(((PatrolEnemyStateGameObject*)otherObject)
							->GetRoute()[((PatrolEnemyStateGameObject*)otherObject)->GetCurrentDest()] + Vector3(0, 10, 0));
				}
				else if (dynamic_cast<BehaviourTreeEnemy*>(otherObject)) {
					((BehaviourTreeEnemy*)otherObject)->GetTransform().SetPosition(((BehaviourTreeEnemy*)otherObject)->GetStartPosition());
					((BehaviourTreeEnemy*)otherObject)->SetReset(true);
				}
				else if (!dynamic_cast<StateGameObject*>(otherObject))		// Don't ever delete state objects
					otherObject->SetIsActive(false);
			}
		};
	}
}