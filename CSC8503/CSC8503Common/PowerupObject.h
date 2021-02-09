/*		 Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Powerup Object Definition		 */
#pragma once
#include "PlayerObject.h"
#include "PickupObject.h"
namespace NCL {
	namespace CSC8503 {
		class PowerupObject : public PickupObject {
		public:
			PowerupObject() {
				name = "Bonus";
			}
			/* If we collide with a player or AI, give them a speed boost */
			void OnCollisionBegin(GameObject* otherObject) override {
				if (dynamic_cast<PlayerObject*>(otherObject) || dynamic_cast<EnemyStateGameObject*>(otherObject) ||
					dynamic_cast<BehaviourTreeEnemy*>(otherObject))
					isActive = false;
				if (dynamic_cast<EnemyStateGameObject*>(otherObject) || dynamic_cast<BehaviourTreeEnemy*>(otherObject)) {
					otherObject->SetPowerUpTimer(5.0f);
					otherObject->GetRenderObject()->SetColour(Vector4(10, 1, 0, 1));
				}
				else if (dynamic_cast<PlayerObject*>(otherObject)) {
					otherObject->SetPowerUpTimer(5.0f);
					otherObject->GetRenderObject()->SetColour(Vector4(0, 1, 10, 1));
				}
			}
		};
	}
}