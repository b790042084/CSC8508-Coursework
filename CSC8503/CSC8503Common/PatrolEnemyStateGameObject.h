/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		Pathfinding Enemy State Game Object Definition	 */
#pragma once
#include "EnemyStateGameObject.h"
#include "Debug.h"
#include "NavigationGrid.h"
namespace NCL {
	namespace CSC8503 {
		class PatrolEnemyStateGameObject : public EnemyStateGameObject {
		public:
			PatrolEnemyStateGameObject(vector<Vector3> positions);
			vector<Vector3> GetRoute() const {
				return route;
			}
			int GetCurrentDest() const {
				return currentDest;
			}
		protected:
			void Patrol(float dt);
			void DisplayRoute();
			vector<Vector3> route;
			State* patrolState;
			int currentDest;
			float routeTimeout;
			bool backwards;
		};
	}
}
