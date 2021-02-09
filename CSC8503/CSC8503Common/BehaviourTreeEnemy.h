/*         Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Behaviour Tree Enemy Definition			 */
#pragma once
#include "GameObject.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"
#include "Debug.h"
namespace NCL {
	namespace CSC8503 {
		enum class behaviour { IDLE, MOVING, SCANNING, CHASING, RETURN };	// Enum represting current behaviour
		class BehaviourTreeEnemy : public GameObject {
		public:
			BehaviourTreeEnemy(Vector3 val);
			void Update(float dt);
			bool GetRayCast() const {
				return rayCast;
			}
			void SetFoundObject(GameObject* o) {
				foundObject = o;
			}
			float GetRayTime() const {
				return rayTime;
			}
			void SetRayTime(float val) {
				rayTime = val;
			}
			Vector3 GetStartPosition() const {
				return startPos;
			}
			void SetReset(bool val) {
				reset = val;
			}
			void SetDisplayRoute(bool val) {
				displayPath = val;
			}
			void DisplayRoute();
			string BehaviourToString() const;
		protected:
			bool reset;
			Vector3 startPos;
			float idleTimer;
			Vector3 travelDir;
			BehaviourState state;
			BehaviourSequence* rootSequence;
			Vector3 randomLocation;
			float searchTime;
			float speed;
			float halfTime;
			bool rayCast;
			float rayTime;
			bool turning;
			GameObject* foundObject;
			behaviour currentBehaviour;
			bool displayPath;
		};
	}
}