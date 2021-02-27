/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game definition		 */
#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/StateGameObject.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/PathFindingEnemyStateGameObject.h"
#include "../CSC8503Common/PatrolEnemyStateGameObject.h"
#include "../CSC8503Common/PlatformStateGameObject.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/IceObject.h"
#include "../CSC8503Common/LavaObject.h"
#include "../CSC8503Common/FloorObject.h"
#include "../CSC8503Common/TrampolineObject.h"
#include "../CSC8503Common/CubeObject.h"
#include "../CSC8503Common/SphereObject.h"
#include "../CSC8503Common/RotatingCubeObject.h"
#include "../CSC8503Common/SpringObject.h"
#include "../CSC8503Common/PlayerObject.h"
#include "../CSC8503Common/EnemyObject.h"
#include "../CSC8503Common/CoinObject.h"
#include "../CSC8503Common/PowerupObject.h"
#include "../CSC8503Common/CapsuleObject.h"
#include "../CSC8503Common/FinishObject.h"
#include "../CSC8503Common/Pushdownstate.h"
#include "../CSC8503Common/PauseScreen.h"
#include "../CSC8503Common/GlobalVariables.h"
#include "../CSC8503Common/BehaviourTreeEnemy.h"
#include <ctype.h>
#include "PxPhysicsAPI.h"
#include "SnippetPrint.h"
#include "SnippetPVD.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerObject;
		enum class FinishType { INGAME, TIMEOUT, WIN, LOSE };
		class TutorialGame {
		public:
			TutorialGame();
			GameObject* AddPxCubeToWorld(GameObject* cube, PxRigidActor* body, const Vector3& position, Vector3 dimensions);
			GameObject* AddPxSphereToWorld(GameObject* sphere, PxRigidActor* body, const Vector3& position, float radius);
			GameObject* AddPxCapsuleToWorld(GameObject* capsule, PxRigidActor* body, const Vector3& position, float radius, float halfHeight);
			GameObject* AddPxFloorToWorld(GameObject* cube, PxRigidStatic* body, const Vector3& position, Vector3 dimensions);

			GameObject* AddPxPickupToWorld(GameObject* p, PxRigidStatic* body, const Vector3& position, float radius);
			GameObject* AddPxPlayerToWorld(GameObject* p, PxRigidActor* body, const Vector3& position, float scale);
			GameObject* AddPxEnemyToWorld(GameObject* e, PxRigidActor* body, const Vector3& position, float scale);


			~TutorialGame();

			void Update(float dt) {
				UpdateLevel(dt);
				physics->ClearDeletedCollisions();
				Debug::FlushRenderables(dt);
				world->UpdateWorld(dt);
				renderer->Update(dt);
				renderer->Render();
			}

			void UpdateMenu(float dt);
			void UpdateLevel(float dt);
			void DrawDebugInfo();
			void CheckFinished(float dt);
			void UpdateLevel2(float dt);
			void EnemyRaycast();

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			void InitFloors(int level);
			void CreateMaze();
			void InitGameExamples(int level);
			void InitPlayer();
			void InitGameObstacles(int level);
			
			void AddBridgeToWorld(Vector3 startPos);

			bool SelectObject();
			void DebugObjectMovement();
			void LockedObjectMovement(float dt);

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool useBroadphase;
			bool inSelectionMode;
			float forceMagnitude;
			float reloadTime;
			float timeOut;
			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLTexture* floorTex = nullptr;
			OGLTexture* lavaTex = nullptr;
			OGLTexture* iceTex = nullptr;
			OGLTexture* trampolineTex = nullptr;
			OGLTexture* obstacleTex = nullptr;
			OGLTexture* woodenTex = nullptr;
			OGLTexture* finishTex = nullptr;
			OGLTexture* menuTex = nullptr;
			OGLTexture* plainTex = nullptr;
			OGLTexture* wallTex = nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
		
			PlayerObject* player;
			vector<EnemyStateGameObject*> enemies;
			vector<PlatformStateGameObject*> platforms;
			int currentLevel;
			bool lockedOrientation;

			float textSize = 15.0f;
			int currentlySelected;
			vector<GameObject*> menuEnemies;
			vector<GameObject*> menuPlayers;

			int avgFps;
			int framesPerSecond;
			float fpsTimer;

			CameraState camState;
			vector<GameObject*> projectiles;
			FinishType finish;

			vector<BehaviourTreeEnemy*> behaviourEnemies;

			GameObject* currentEnemy = nullptr;
		};
	}
}

