/*		 Created  By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Coin Object Definition		 */
#pragma once
#include "PickupObject.h"
namespace NCL {
	namespace CSC8503 {
		class CoinObject : public PickupObject {
		public:
			CoinObject() {
				name = "Bonus";
			}
			/* If we collide with a player or enemy delete us */
			void OnCollisionBegin(GameObject* otherObject) override {
				if (dynamic_cast<PlayerObject*>(otherObject) || dynamic_cast<EnemyStateGameObject*>(otherObject) ||
					dynamic_cast<BehaviourTreeEnemy*>(otherObject))
					isActive = false;
				if (dynamic_cast<PlayerObject*>(otherObject))
					((PlayerObject*)otherObject)->BonusAcquired();		// Give the player some points
			}
		};
	}
}