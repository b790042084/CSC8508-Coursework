/*			Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Finish Object Definition		 */
#pragma once
#include "FloorObject.h"
#include "PathFindingEnemyStateGameObject.h"
namespace NCL {
	namespace CSC8503 {
		class FinishObject : public FloorObject {
		public:
			FinishObject() {
				name = "Finish";
			}
			/* If the enemy or player has collided, let the game know */
			void OnCollisionBegin(GameObject* otherObject) override {
				if (dynamic_cast<PlayerObject*>(otherObject))
					((PlayerObject*)otherObject)->SetFinished(true);
				else if (dynamic_cast<PathFindingEnemyStateGameObject*>(otherObject))
					((PathFindingEnemyStateGameObject*)otherObject)->SetFinished(true);
			}
		};
	}
}