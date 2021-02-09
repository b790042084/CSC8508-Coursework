/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game Implementation		 */
#include "TutorialGame.h"
using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);
	physics->SetDampingFactor(0.3);
	forceMagnitude = 10.0f;
	useGravity = true;
	physics->UseGravity(useGravity);
	useBroadphase = true;
	physics->UseBroadphase(useBroadphase);
	inSelectionMode = false;
	reloadTime = 0.0f;
	timeOut = 0.0f;
	Debug::SetRenderer(renderer);
	player = nullptr;
	lockedOrientation = true;
	currentLevel = 0;
	currentlySelected = 1;
	avgFps = 1.0f;
	framesPerSecond = 0;
	fpsTimer = 1.0f;
	finish = FinishType::INGAME;
	InitialiseAssets();
}

/* Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like! */
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	obstacleTex = (OGLTexture*)TextureLoader::LoadAPITexture("obstacle.png");
	floorTex = (OGLTexture*)TextureLoader::LoadAPITexture("platform.png");
	lavaTex = (OGLTexture*)TextureLoader::LoadAPITexture("lava.png");
	trampolineTex = (OGLTexture*)TextureLoader::LoadAPITexture("trampoline.png");
	iceTex = (OGLTexture*)TextureLoader::LoadAPITexture("ice.png");
	woodenTex = (OGLTexture*)TextureLoader::LoadAPITexture("wood.png");
	finishTex = (OGLTexture*)TextureLoader::LoadAPITexture("finish.png");
	menuTex = (OGLTexture*)TextureLoader::LoadAPITexture("menu.png");
	plainTex = (OGLTexture*)TextureLoader::LoadAPITexture("plain.png");
	wallTex = (OGLTexture*)TextureLoader::LoadAPITexture("wall.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete obstacleTex;
	delete floorTex;
	delete lavaTex;
	delete trampolineTex;
	delete iceTex;
	delete woodenTex;
	delete finishTex;
	delete menuTex;
	delete basicShader;

	delete physics;
	//delete renderer;
	delete world;
}

/* Updates all menu items */
void TutorialGame::UpdateMenu(float dt) {
	/* Update our menu objects */
	renderer->DrawString("Choose Level", Vector2(35, 5), Debug::WHITE, 30.0f);
	currentlySelected == 1 ? renderer->DrawString("Level 1: Single Player Obstacle Course", Vector2(0, 15), { 0,1,0,1 }) :
		renderer->DrawString("Level 1: Single Player Obstacle Course", Vector2(0, 15), Debug::WHITE);
	currentlySelected > 1 ? renderer->DrawString("Level 2: Multiplayer Maze", Vector2(0, 45), { 0,1,0,1 }) :
		renderer->DrawString("Level 2: Multiplayer Maze", Vector2(0, 45), Debug::WHITE);
	renderer->DrawString("Enemy Opponents: ", Vector2(0, 50), Debug::WHITE);
	currentlySelected == 2 ? renderer->DrawString("[0]", Vector2(30, 50), { 0,1,0,1 }) :
		renderer->DrawString("[0]", Vector2(30, 50), Debug::WHITE);
	currentlySelected == 3 ? renderer->DrawString("[1]", Vector2(36, 50), { 0,1,0,1 }) :
		renderer->DrawString("[1]", Vector2(36, 50), Debug::WHITE);
	currentlySelected == 4 ? renderer->DrawString("[2]", Vector2(42, 50), { 0,1,0,1 }) :
		renderer->DrawString("[2]", Vector2(42, 50), Debug::WHITE);
	currentlySelected == 5 ? renderer->DrawString("[3]", Vector2(48, 50), { 0,1,0,1 }) :
		renderer->DrawString("[3]", Vector2(48, 50), Debug::WHITE);
	renderer->DrawString("Exit(ESC)", Vector2(80, 5));
	renderer->DrawString("Choose[UP][DOWN]", Vector2(70, 90));
	renderer->DrawString("Select[ENTER]", Vector2(75, 95));

	/* Change what is currently highlighted depending on the current values */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
			currentlySelected = currentlySelected == 1 ? 5 : currentlySelected - 1;
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
			currentlySelected = currentlySelected == 5 ? 1 : currentlySelected + 1;
		switch (currentlySelected) {
		case 1:
			menuPlayers.at(1)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			menuPlayers.at(0)->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
			menuEnemies.at(0)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			menuEnemies.at(1)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			menuEnemies.at(2)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			break;
		case 2:
			menuPlayers.at(0)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			menuPlayers.at(1)->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
			menuEnemies.at(0)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			break;
		case 3:
			menuEnemies.at(0)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(1)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			break;
		case 4:
			menuEnemies.at(1)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(2)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			break;
		case 5:
			menuPlayers.at(0)->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			menuPlayers.at(1)->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
			menuEnemies.at(0)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(1)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			menuEnemies.at(2)->GetRenderObject()->SetColour((Vector4(1, 0, 0, 1)));
			break;
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
		currentLevel = currentlySelected >= 2 ? 2 : 1;
		InitWorld();		// Clears all objects and resets to new level
	}
}

/* Logic for updating level 1 or level 2 */
void TutorialGame::UpdateLevel(float dt) {
	/* Enter debug mode? */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (dynamic_cast<EnemyStateGameObject*>(selectionObject))
			((EnemyStateGameObject*)selectionObject)->SetDisplayDirection(inSelectionMode);
		if (dynamic_cast<BehaviourTreeEnemy*>(selectionObject))
			((BehaviourTreeEnemy*)selectionObject)->SetDisplayRoute(inSelectionMode);
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	/* Debug mode selection */
	inSelectionMode ? renderer->DrawString("Change to play mode(Q)", Vector2(69, 10), Debug::WHITE, textSize) :
		renderer->DrawString("Change to debug mode(Q)", Vector2(68, 10), Debug::WHITE, textSize);
	if (inSelectionMode) {
		UpdateKeys();
		SelectObject();
		DrawDebugInfo();
		world->ShowFacing();
	}
	else {
		if (player) {
			renderer->DrawString("Score:" + std::to_string(player->GetScore()), Vector2(0, 10), Debug::WHITE, textSize);
			player->Update(dt);
		}
	}

	/* Camera state displayed to user */
	switch (camState) {
	case CameraState::FREE:
		renderer->DrawString("Change to Global Camera[1]", Vector2(62, 20), Debug::WHITE, textSize);
		break;
	case CameraState::GLOBAL1:
		renderer->DrawString("Change to Free Camera[1]", Vector2(65, 20), Debug::WHITE, textSize);
		break;
	case CameraState::GLOBAL2:
		renderer->DrawString("Change to Free Camera[1]", Vector2(65, 20), Debug::WHITE, textSize);
		break;
	case CameraState::THIRDPERSON:
		renderer->DrawString("Change to Topdown Camera[1]", Vector2(61, 20), Debug::WHITE, textSize);
		break;
	case CameraState::TOPDOWN:
		renderer->DrawString("Change to Thirdperson Camera[1]", Vector2(56, 20), Debug::WHITE, textSize);
		break;
	}

	/* Change Camera */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1)) {
		switch (camState) {
		case CameraState::FREE:
			camState = currentLevel == 1 ? CameraState::GLOBAL1 : CameraState::GLOBAL2;
			break;
		case CameraState::GLOBAL1:
			InitCamera();
			camState = CameraState::FREE;
			break;
		case CameraState::GLOBAL2:
			InitCamera();
			camState = CameraState::FREE;
			break;
		}
		world->GetMainCamera()->SetState(camState);
	}

	physics->Update(dt);		// Update our physics

	/* Change how we move the camera dependng if we have a locked object */
	if (lockedObject) {
		world->GetMainCamera()->UpdateCameraWithObject(dt, lockedObject);
		if (lockedOrientation)
			lockedObject->GetTransform().SetOrientation(Matrix4::Rotation(world->GetMainCamera()->GetYaw(), { 0, 1, 0 }));
	}
	else if (!inSelectionMode || camState == CameraState::GLOBAL1 || camState == CameraState::GLOBAL2)
		world->GetMainCamera()->UpdateCamera(dt);

	renderer->DrawString("Exit to Menu (ESC)", Vector2(75, 5), Debug::WHITE, textSize);
	renderer->DrawString("Pause(P)", Vector2(88, 15), Debug::WHITE, textSize);

	currentLevel == 1 ? UpdateLevel1(dt) : UpdateLevel2(dt);		// Update level specific game logic 

	lockedObject ? LockedObjectMovement(dt) : DebugObjectMovement();
	CheckFinished(dt);		// Should the game have ended? 
}

/* Draws debug information to screen, and will display selected object properties */
void TutorialGame::DrawDebugInfo() {
	world->GetShuffleObjects() ? renderer->DrawString("Shuffle Objects(F1):On", Vector2(0, 10), Debug::WHITE, textSize) :
		renderer->DrawString("Shuffle Objects(F1):Off", Vector2(0, 10), Debug::WHITE, textSize);
	world->GetShuffleConstraints() ? renderer->DrawString("Shuffle Constraints(F2):On", Vector2(0, 15), Debug::WHITE, textSize) :
		renderer->DrawString("Shuffle Constraints(F2):Off", Vector2(0, 15), Debug::WHITE, textSize);

	useGravity ? renderer->DrawString("Gravity(G):On", Vector2(0, 20), Debug::WHITE, textSize) :
		renderer->DrawString("Gravity(G):Off", Vector2(0, 20), Debug::WHITE, textSize);
	renderer->DrawString("Click Force(Scroll Wheel):" + std::to_string((int)forceMagnitude), Vector2(0, 25), Debug::WHITE, textSize);
	renderer->DrawString("Constraint Iteration Count(I/O):" + std::to_string(physics->GetConstraintIterationCount()), Vector2(0, 30), Debug::WHITE, textSize);

	if (physics->GetBroadPhase()) {
		renderer->DrawString("Broadphase(B):On", Vector2(76, 80), Debug::WHITE, textSize);
		renderer->DrawString("Collisions Tested:" + std::to_string(physics->GetBroadPhaseCollisionsTested()), Vector2(68, 90), Debug::WHITE, textSize);
	}
	else {
		renderer->DrawString("Broadphase(B):Off", Vector2(76, 80), Debug::WHITE, textSize);
		renderer->DrawString("Collisions Tested:" + std::to_string(physics->GetBasicCollisionsTested()), Vector2(68, 90), Debug::WHITE, textSize);
	}
	if (lockedObject) {
		renderer->DrawString("Unlock object(L)", Vector2(0, 35), Debug::WHITE, textSize);
		lockedOrientation ? renderer->DrawString("Lock object orientation(K): On", Vector2(0, 40), Debug::WHITE, textSize) :
			renderer->DrawString("Lock object orientation(K): Off", Vector2(0, 40), Debug::WHITE, textSize);
	}
	else
		renderer->DrawString("Lock selected object(L)", Vector2(0, 35), Debug::WHITE, textSize);
	renderer->DrawString("Current Collisions:" + std::to_string(physics->GetCollisionsSize()), Vector2(68, 95), Debug::WHITE, textSize);
	renderer->DrawString("Total Objects:" + std::to_string(world->GetTotalWorldObjects()), Vector2(75, 85), Debug::WHITE, textSize);

	/* If selected an object display all its physical properties */
	if (selectionObject) {
		/* Display state machine information */
		if (dynamic_cast<StateGameObject*>(selectionObject)) {
			renderer->DrawString("State:" + ((StateGameObject*)selectionObject)->StateToString(), Vector2(0, 55), Debug::WHITE, textSize);
			selectionObject->GetPowerUpTimer() > 0.0f ? renderer->DrawString("Powered Up: Yes", Vector2(0, 50), Debug::WHITE, textSize) :
				renderer->DrawString("Powered Up: No", Vector2(0, 50), Debug::WHITE, textSize);
		}
		else if (dynamic_cast<BehaviourTreeEnemy*>(selectionObject)) {
			renderer->DrawString("Current Behaviour:" + ((BehaviourTreeEnemy*)selectionObject)->BehaviourToString(), Vector2(0, 55), Debug::WHITE, textSize);
			selectionObject->GetPowerUpTimer() > 0.0f ? renderer->DrawString("Powered Up: Yes", Vector2(0, 50), Debug::WHITE, textSize) :
				renderer->DrawString("Powered Up: No", Vector2(0, 50), Debug::WHITE, textSize);
		}
		else if (dynamic_cast<PlayerObject*>(selectionObject)) {
			selectionObject->GetPowerUpTimer() > 0.0f ? renderer->DrawString("Powered Up: Yes", Vector2(0, 55), Debug::WHITE, textSize) :
				renderer->DrawString("Powered Up: No", Vector2(0, 55), Debug::WHITE, textSize);
		}
		renderer->DrawString("Selected Object:" + selectionObject->GetName(), Vector2(0, 60), Debug::WHITE, textSize);
		renderer->DrawString("Position:" + selectionObject->GetTransform().GetPosition().ToString(), Vector2(0, 65), Debug::WHITE, textSize);
		renderer->DrawString("Orientation:" + selectionObject->GetTransform().GetOrientation().ToEuler().ToString(), Vector2(0, 70), Debug::WHITE, textSize);
		renderer->DrawString("Linear Velocity:" + selectionObject->GetPhysicsObject()->GetLinearVelocity().ToString(), Vector2(0, 75), Debug::WHITE, textSize);
		renderer->DrawString("Angular Veclocity:" + selectionObject->GetPhysicsObject()->GetAngularVelocity().ToString(), Vector2(0, 80), Debug::WHITE, textSize);
		renderer->DrawString("Inverse Mass:" + std::to_string(selectionObject->GetPhysicsObject()->GetInverseMass()), Vector2(0, 85), Debug::WHITE, textSize);
		renderer->DrawString("Friction:" + std::to_string(selectionObject->GetPhysicsObject()->GetFriction()), Vector2(0, 90), Debug::WHITE, textSize);
		renderer->DrawString("Elasticity:" + std::to_string(selectionObject->GetPhysicsObject()->GetElasticity()), Vector2(0, 95), Debug::WHITE, textSize);
	}
}

/* Checks whether an enemy or the player has reached the finish, or score is 0 */
void TutorialGame::CheckFinished(float dt) {
	if (player && !inSelectionMode) {
		if ((player->GetScore() <= 0 || player->GetFinished()) && finish == FinishType::INGAME) {
			if (player->GetScore() <= 0)
				finish = FinishType::TIMEOUT;
			else if (player->GetFinished())
				finish = FinishType::WIN;
		}
	}

	for (auto& e : enemies) {
		if (e->GetFinished() && finish == FinishType::INGAME)
			finish = FinishType::LOSE;		// If any of our pathfinding enemies have won
	}

	switch (finish) {
	case FinishType::INGAME:
		if (player && !inSelectionMode)
			player->DecreaseScore(dt);
		break;
	case FinishType::TIMEOUT:
		renderer->DrawString("Time Up - Game Over", Vector2(30, 40), Vector4(1, 0, 0, 1), 25.0f);
		break;
	case FinishType::WIN:
		renderer->DrawString("Level Finished", Vector2(34, 40), Vector4(0, 1, 0, 1), 25.0f);
		renderer->DrawString("Score: " + std::to_string(player->GetScore()), Vector2(42, 45), Vector4(1, 1, 0, 1));
		lockedOrientation = false;
		break;
	case FinishType::LOSE:
		renderer->DrawString("AI Won - Game Over", Vector2(32, 40), Vector4(1, 0, 0, 1), 25.0f);
		if (player)
			renderer->DrawString("Score:" + std::to_string(player->GetScore()), Vector2(42, 45), Vector4(1, 1, 0, 1));
		break;
	}

	if (finish != FinishType::INGAME) {
		timeOut += dt;
		renderer->DrawString("Exiting to Menu in:" + std::to_string((int)(5.0f - timeOut)) + "s", Vector2(30, 50));
	}
}

/* Updates specific logic for obstacle course */
void TutorialGame::UpdateLevel1(float dt) {
	for (auto& p : platforms)
		p->Update(dt);

	for (auto& e : enemies) {
		e->Update(dt);
		currentEnemy = e;
		EnemyRaycast();
	}

	for (auto& e : behaviourEnemies) {
		e->Update(dt);
		currentEnemy = e;
		EnemyRaycast();
	}

	reloadTime += dt;
	if (reloadTime > 2.0f) {
		FireObjects();
		reloadTime = 0.0f;
	}

	for (auto& o : projectiles)
		o->StepTimeAlive(dt);		// Any objects that have been dynamically added should be removed eventually

	/* Setting checkpoints and camera switches */
	if (player) {
		switch ((int)player->GetTransform().GetPosition().z) {
		case 0:
			player->SetSpawnPos({ 0, 10, 50 });
			break;
		case -150:
			camState = CameraState::TOPDOWN;
			world->GetMainCamera()->SetState(camState);
			break;
		case -265:
			player->SetSpawnPos({ 0, 10, -265 });
			break;
		case -445:
			camState = CameraState::THIRDPERSON;
			world->GetMainCamera()->SetState(camState);
			break;
		case -620:
			player->SetSpawnPos({ 0, 10, -620 });
			break;
		case -840:
			player->SetSpawnPos({ 0, 10, -840 });
			camState = CameraState::TOPDOWN;
			world->GetMainCamera()->SetState(camState);
			break;
		case -990:
			if ((int)player->GetTransform().GetPosition().x == -30 && (int)player->GetTransform().GetPosition().y > 20)
				player->SetSpawnPos({ -30, 50, -990 });
			else if ((int)player->GetTransform().GetPosition().x >= 120 && (int)player->GetTransform().GetPosition().y > 20) {
				camState = CameraState::TOPDOWN;
				world->GetMainCamera()->SetState(camState);
			}
			else {
				camState = CameraState::THIRDPERSON;
				world->GetMainCamera()->SetState(camState);
			}
			break;
		}
	}
}

/* Fires objects across the platform */
void TutorialGame::FireObjects() {
	SphereObject* sphere = new SphereObject;
	rand() % 2 ? projectiles.push_back(AddSphereToWorld(sphere, Vector3(90, 5, ((rand() % (200 - 210 + 1)) + 200) * -1), 3)) :
		projectiles.push_back(AddSphereToWorld(sphere, Vector3(90, 5, ((rand() % (250 - 240 + 1)) + 240) * -1), 3));
	sphere->GetPhysicsObject()->SetLinearVelocity(Vector3(-150, 0, 0));

	switch (rand() % 2) {
	case 0:
		projectiles.push_back(AddSphereToWorld(new SphereObject, platforms.at(1)->GetTransform().GetPosition() - Vector3(0, 10, 0), 2));
		break;
	case 1:
		CapsuleObject * capsule = new CapsuleObject;
		projectiles.push_back(AddCapsuleToWorld(capsule, platforms.at(1)->GetTransform().GetPosition() - Vector3(0, 10, 0), 4, 2));
		capsule->GetTransform().SetOrientation(Matrix4::Rotation(90, Vector3(0, 0, 1)));
		break;
	}
}

/* Updates the maze level */
void TutorialGame::UpdateLevel2(float dt) {
	if (player)
		player->SetSpawnPos({ 0, 10, 0 });
	for (auto& e : enemies) {
		e->Update(dt);
		currentEnemy = e;
		EnemyRaycast();
	}
}

/* Enemies fire 13 rays every 0.25s across a range to detect objects of interest */
void TutorialGame::EnemyRaycast() {
	float fov = -1, numRay = 13;
	if (dynamic_cast<EnemyStateGameObject*>(currentEnemy)) {
		if (((EnemyStateGameObject*)currentEnemy)->GetRayTime() < 0.0f) {
			for (int i = 0; i < numRay; ++i) {
				Ray ray(((EnemyStateGameObject*)currentEnemy)->GetTransform().GetPosition(),
					((EnemyStateGameObject*)currentEnemy)->GetTransform().GetOrientation() * Vector3(fov, 0, -1));
				RayCollision closestCollision;
				if (world->Raycast(ray, closestCollision, currentEnemy, true) && closestCollision.rayDistance < 80.0f) {
					/* If enemy sees a player or pickup item */
					if (dynamic_cast<PlayerObject*>((GameObject*)closestCollision.node) ||
						dynamic_cast<PickupObject*>((GameObject*)closestCollision.node)) {
						((EnemyStateGameObject*)currentEnemy)->AddFollowObject((GameObject*)closestCollision.node);
						((EnemyStateGameObject*)currentEnemy)->SetFollowTimeOut(5.0f);
					}
				}
				if (currentEnemy == selectionObject && inSelectionMode)
					closestCollision.node ? Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::GREEN) :
					Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + (ray.GetDirection() * 80), Debug::RED);
				fov += 2 / (numRay - 1);
			}
			((EnemyStateGameObject*)currentEnemy)->SetRayTime(0.25f);
		}
	}
	else if (dynamic_cast<BehaviourTreeEnemy*>(currentEnemy)) {
		if (((BehaviourTreeEnemy*)currentEnemy)->GetRayTime() < 0.0f && ((BehaviourTreeEnemy*)currentEnemy)->GetRayCast()) {
			for (int i = 0; i < numRay; ++i) {
				Ray ray(((BehaviourTreeEnemy*)currentEnemy)->GetTransform().GetPosition(),
					((BehaviourTreeEnemy*)currentEnemy)->GetTransform().GetOrientation() * Vector3(fov, 0, -1));
				RayCollision closestCollision;
				if (world->Raycast(ray, closestCollision, currentEnemy, true) && closestCollision.rayDistance < 80.0f) {
					/* If enemy sees a player or pickup item */
					if (dynamic_cast<PlayerObject*>((GameObject*)closestCollision.node) ||
						dynamic_cast<PickupObject*>((GameObject*)closestCollision.node)) {
						((BehaviourTreeEnemy*)currentEnemy)->SetFoundObject((GameObject*)closestCollision.node);
					}
				}
				if (currentEnemy == selectionObject && inSelectionMode)
					closestCollision.node ? Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::GREEN) :
					Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + (ray.GetDirection() * 80), Debug::RED);
				fov += 2 / (numRay - 1);
			}
			((BehaviourTreeEnemy*)currentEnemy)->SetRayTime(0.25f);
		}
	}
}

/* In debug mode we can change some of the backend physics engine with some key presses */
void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::B)) {
		useBroadphase = !useBroadphase;
		physics->UseBroadphase(useBroadphase);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::I))
		physics->IncreaseIterationCount();
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O))
		physics->DecreaseIterationCount();

	/*Running certain physics updates in a consistent order might cause some
	bias in the calculations - the same objects might keep 'winning' the constraint
	allowing the other one to stretch too much etc. Shuffling the order so that it
	is random every frame can help reduce such bias. */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
		world->ShuffleObjects(!world->GetShuffleObjects());
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2))
		world->ShuffleConstraints(!world->GetShuffleConstraints());

	/* If an object has been clicked, it can be pushed with the right mouse button, by an amount
	determined by the scroll wheel. */
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 10.0f;
}

/* Initialise camera to default location */
void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(1600.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 50, 80));
	world->GetMainCamera()->SetYaw(0);
	world->GetMainCamera()->SetPitch(0);
	lockedObject = nullptr;
}

/* Initialise all the elements contained within the world */
void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	InitFloors(currentLevel);
	InitGameExamples(currentLevel);
	InitGameObstacles(currentLevel);
}

/* Place all the levels solid floors */
void TutorialGame::InitFloors(int level) {
	switch (level) {
	case 0:
		/* Background object */
		AddFloorToWorld(new FloorObject, Vector3(0, 50, 0), Vector3(80, 40, 1))->GetRenderObject()->SetDefaultTexture(menuTex);
		break;
	case 1:
		/* Lava */
		AddFloorToWorld(new LavaObject, Vector3(0, -60, -400), Vector3(500, 1, 1000));

		/* Starting Platform */
		AddFloorToWorld(new FloorObject, Vector3(0, 0, 50), Vector3(25, 1, 25));

		/* Walls */
		AddFloorToWorld(new FloorObject, Vector3(-25, 2, 0), Vector3(1, 2, 24));
		AddFloorToWorld(new FloorObject, Vector3(25, 2, 0), Vector3(1, 2, 24));
		AddFloorToWorld(new FloorObject, Vector3(-15, 2, 25), Vector3(11, 2, 1));
		AddFloorToWorld(new FloorObject, Vector3(15, 2, 25), Vector3(11, 2, 1));
		AddFloorToWorld(new FloorObject, Vector3(-15, 2, -25), Vector3(11, 2, 1));
		AddFloorToWorld(new FloorObject, Vector3(15, 2, -25), Vector3(11, 2, 1));

		/* First platforms */
		AddFloorToWorld(new FloorObject, Vector3(0, 0, 0), Vector3(25, 1, 25));
		AddFloorToWorld(new FloorObject, Vector3(0, 10, -107), Vector3(25, 1, 40), Matrix4::Rotation(15, { 1, 0, 0 }));
		AddFloorToWorld(new FloorObject, Vector3(0, 20.4, -150.2), Vector3(25, 1, 5));
		AddFloorToWorld(new IceObject, Vector3(0, 10, -175), Vector3(25, 1, 23), Matrix4::Rotation(-27, { 1, 0, 0 }));

		/* Projectile Wall */
		AddFloorToWorld(new FloorObject, Vector3(120, 5, -225), Vector3(1, 5, 30));

		/* Sphere rolling floors */
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -205), Vector3(100, 1, 10));
		AddFloorToWorld(new FloorObject, Vector3(-90, 0, -225), Vector3(10, 1, 10));
		AddFloorToWorld(new FloorObject, Vector3(90, 0, -225), Vector3(10, 1, 10));
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -245), Vector3(100, 1, 10));
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -265), Vector3(10, 1, 10));

		/* Ice Shortcut */
		AddFloorToWorld(new IceObject, Vector3(0, 0, -225), Vector3(5, 1, 5));

		/* High elasticity trampoline */
		AddFloorToWorld(new TrampolineObject, Vector3(0, -30, -315), Vector3(10, 1, 10));
		AddFloorToWorld(new TrampolineObject, Vector3(0, -30, -395), Vector3(10, 1, 10));

		/* Regular Platform*/
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -445), Vector3(10, 1, 10));

		/* Low Friction Ice */
		AddFloorToWorld(new IceObject, Vector3(40, 0, -475), Vector3(30, 1, 5), Matrix4::Rotation(30, { 0, 1, 0 }));
		AddFloorToWorld(new IceObject, Vector3(0, 0, -515), Vector3(5, 1, 60), Matrix4::Rotation(75, { 0, 1, 0 }));
		AddFloorToWorld(new IceObject, Vector3(-40, 0, -555), Vector3(30, 1, 5), Matrix4::Rotation(150, { 0, -1, 0 }));

		/* Regular Platform */
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -605), Vector3(10, 1, 30));
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -645), Vector3(80, 1, 10));
		AddFloorToWorld(new FloorObject, Vector3(-90, 0, -735), Vector3(10, 1, 100));
		AddFloorToWorld(new FloorObject, Vector3(90, 0, -735), Vector3(10, 1, 100));
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -825), Vector3(80, 1, 10));
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -865), Vector3(5, 1, 30));
		AddFloorToWorld(new FloorObject, Vector3(-25, 0, -900), Vector3(30, 1, 5));

		AddFloorToWorld(new FloorObject, Vector3(-60, 0, -915), Vector3(5, 1, 20));
		AddFloorToWorld(new FloorObject, Vector3(-60, -20, -1005), Vector3(5, 1, 25));
		AddFloorToWorld(new FloorObject, Vector3(-59, -20, -1055), Vector3(25, 1, 25));
		AddFloorToWorld(new FloorObject, Vector3(-59, -18, -1055), Vector3(5, 2, 5));

		/* Patrol Enemy Walls */
		AddFloorToWorld(new FloorObject, Vector3(-85, -18, -1055), Vector3(1, 3, 25));
		AddFloorToWorld(new FloorObject, Vector3(-35, -18, -1055), Vector3(1, 3, 25));
		AddFloorToWorld(new FloorObject, Vector3(-75.5, -18, -1029), Vector3(10.5, 3, 1));
		AddFloorToWorld(new FloorObject, Vector3(-44.5, -18, -1029), Vector3(10.5, 3, 1));
		AddFloorToWorld(new FloorObject, Vector3(-60, -18, -1080), Vector3(26, 3, 1));

		/* Spring Walls */
		AddFloorToWorld(new FloorObject, Vector3(-5, 2, -865.5), Vector3(1, 3, 30.5));
		AddFloorToWorld(new FloorObject, Vector3(5, 2, -869.5), Vector3(1, 3, 34.5));
		AddFloorToWorld(new FloorObject, Vector3(-65.5, 2, -915.5), Vector3(1, 3, 20));
		AddFloorToWorld(new FloorObject, Vector3(-36.25, 2, -895), Vector3(30.25, 3, 1));
		AddFloorToWorld(new FloorObject, Vector3(-25.25, 2, -905), Vector3(31.25, 3, 1));
		AddFloorToWorld(new FloorObject, Vector3(-55.5, 2, -920.5), Vector3(1, 3, 14.5));

		/* Springboard Walls */
		AddFloorToWorld(new FloorObject, Vector3(-71, -30, -955), Vector3(1, 15, 15));
		AddFloorToWorld(new FloorObject, Vector3(-49, -30, -955), Vector3(1, 15, 15));
		AddFloorToWorld(new FloorObject, Vector3(-60, -30, -939), Vector3(12, 15, 1));
		AddFloorToWorld(new FloorObject, Vector3(-60, -46, -954), Vector3(12, 1, 16));

		/* Rotated Platform */
		AddFloorToWorld(new FloorObject, Vector3(103, 26.5, -990), Vector3(20, 1, 5), Matrix4::Rotation(20, { 0, 0, 1 }));
		AddFloorToWorld(new FloorObject, Vector3(126.2, 33.3, -995), Vector3(5, 1, 35));
		AddFloorToWorld(new FloorObject, Vector3(155, 8.5, -1025), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));
		AddFloorToWorld(new FloorObject, Vector3(155, 8.5, -1010), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));
		AddFloorToWorld(new FloorObject, Vector3(155, 8.5, -995), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));
		AddFloorToWorld(new FloorObject, Vector3(155, 8.5, -980), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));
		AddFloorToWorld(new FloorObject, Vector3(155, 8.5, -965), Vector3(35, 1, 5), Matrix4::Rotation(45, { 0, 0, -1 }));

		/* Finish Platform */
		AddFloorToWorld(new FinishObject, Vector3(204, -50, -995), Vector3(39, 1, 38));
		AddFloorToWorld(new FloorObject, Vector3(204, -52, -995), Vector3(41, 1, 40));

		/* Finish Walls */
		AddFloorToWorld(new FloorObject, Vector3(205, -45, -956), Vector3(40, 6, 1));
		AddFloorToWorld(new FloorObject, Vector3(205, -45, -1034), Vector3(40, 6, 1));
		AddFloorToWorld(new FloorObject, Vector3(164, -45, -995), Vector3(1, 6, 40));
		AddFloorToWorld(new FloorObject, Vector3(244, -45, -995), Vector3(1, 6, 40));
		break;
	case 2:
		/* Lava */
		AddFloorToWorld(new LavaObject, Vector3(0, -60, -500), Vector3(800, 1, 800));

		/* Regular Platform */
		AddFloorToWorld(new FloorObject, Vector3(0, 0, 10), Vector3(30, 1, 40));
		AddFloorToWorld(new FloorObject, Vector3(0, 10, 30), Vector3(3, 1, 3))->GetRenderObject()->SetDefaultTexture(wallTex);		// Starting platform

		/* Walls*/
		AddFloorToWorld(new FloorObject, Vector3(-31, 4, 20), Vector3(1, 5, 31));
		AddFloorToWorld(new FloorObject, Vector3(31, 4, 20), Vector3(1, 5, 31));
		AddFloorToWorld(new FloorObject, Vector3(0, 4, 51), Vector3(32, 5, 1));

		/* Maze Floor */
		AddFloorToWorld(new FloorObject, Vector3(0, 0, -240), Vector3(210, 1, 210));
		CreateMaze();

		/* Finish Platform */
		AddFloorToWorld(new FinishObject, Vector3(0, 1, -240), Vector3(10, 1, 10));
		break;
	}
}

/* Places all walls and obstacles that are found in the grid file */
void TutorialGame::CreateMaze() {
	NavigationGrid grid("MazePath.txt");
	Vector3 offset(220, -20, 460), nodePos, objectPos, icePos;
	float xSize = 10, zSize = 10, iceXSize = 10, iceZsize = 10;

	/* Place all horizontally efficient walls and obstacles */
	for (int y = 0; y < 23; ++y) {
		nodePos = grid.GetNodes()[23 * y].position;
		xSize = 10;
		iceXSize = 10;
		for (int x = 0; x < 23; ++x) {
			objectPos = grid.GetNodes()[(23 * y) + x + 1].position;

			/* Place items and obstacles */
			if (grid.GetNodes()[(23 * y) + x].type == SPRING_NODE)
				AddCubeToWorld(new SpringObject(objectPos - offset + Vector3(-20, -15, 0), 0.002f), objectPos - offset + Vector3(-20, -15, 0), Vector3(8, 5, 1));
			if (grid.GetNodes()[(23 * y) + x].type == COIN_NODE)
				AddPickupToWorld(new CoinObject, objectPos - offset + Vector3(-20, -16, 0));
			if (grid.GetNodes()[(23 * y) + x].type == POWERUP_NODE)
				AddPickupToWorld(new PowerupObject, objectPos - offset + Vector3(-20, -16, 0));

			if (grid.GetNodes()[(23 * y) + x].type == WALL_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 1].type != WALL_NODE || x == 22) && xSize > zSize)
					AddFloorToWorld(new FloorObject, nodePos - offset, { xSize, 20, zSize })->GetRenderObject()->SetDefaultTexture(wallTex);
				else {
					nodePos.x += 10;
					xSize += 10;
				}
			}
			else {
				xSize = 10;
				nodePos = grid.GetNodes()[(23 * y) + x + 1].position;
			}
			if (grid.GetNodes()[(23 * y) + x].type == ICE_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 1].type != ICE_NODE || x == 22) && iceXSize > iceZsize)
					AddFloorToWorld(new IceObject, icePos - offset - Vector3(0, 19, 0), Vector3(iceXSize, 0.1, iceZsize));
				else {
					icePos.x += 10;
					iceXSize += 10;
				}
			}
			else {
				iceXSize = 10;
				icePos = grid.GetNodes()[(23 * y) + x + 1].position;
			}
		}
	}

	/* Place all vertically efficient walls and obstacles */
	xSize = 10;
	iceXSize = 10;
	for (int x = 0; x < 23; ++x) {
		nodePos = grid.GetNodes()[x].position;
		icePos = grid.GetNodes()[x].position;
		zSize = 10;
		for (int y = 0; y < 23; ++y) {
			if (grid.GetNodes()[(23 * y) + x].type == WALL_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 23].type != WALL_NODE || y == 22) && zSize > xSize)
					AddFloorToWorld(new FloorObject, nodePos - offset, { xSize, 20, zSize })->GetRenderObject()->SetDefaultTexture(wallTex);
				else {
					nodePos.z += 10;
					zSize += 10;
				}
			}
			else {
				zSize = 10;
				nodePos = grid.GetNodes()[(23 * y) + x + 23].position;
			}
			if (grid.GetNodes()[(23 * y) + x].type == ICE_NODE) {
				if ((grid.GetNodes()[(23 * y) + x + 23].type != ICE_NODE || y == 22) && iceZsize >= iceXSize)
					AddFloorToWorld(new IceObject, icePos - offset - Vector3(0, 19, 0), Vector3(iceXSize, 0.1, iceZsize));
				else {
					icePos.z += 10;
					iceZsize += 10;
				}
			}
			else {
				iceZsize = 10;
				icePos = grid.GetNodes()[(23 * y) + x + 23].position;
			}
		}
	}
}

/* Initialises all game objects, enemies etc */
void TutorialGame::InitGameExamples(int level) {
	vector<Vector3> positions = { Vector3(-59, -15, -1035), Vector3(-75, -15, -1055),  Vector3(-59, -15, -1075),
		Vector3(-43, -15, -1055), Vector3(-59, -15, -1035) };		// Coordinates for patrol enemy
	switch (level) {
	case 0:
		/* Menu Objects */
		menuPlayers.push_back(AddPlayerToWorld(new PlayerObject, Vector3(-20, 55, 50)));
		menuPlayers.push_back(AddPlayerToWorld(new PlayerObject, Vector3(-20, 46, 50)));
		menuEnemies.push_back(AddEnemyToWorld(new EnemyObject, { -15, 46, 50 }));
		menuEnemies.push_back(AddEnemyToWorld(new EnemyObject, { -10, 46, 50 }));
		menuEnemies.push_back(AddEnemyToWorld(new EnemyObject, { -5, 46, 50 }));
		for (auto& e : menuEnemies) {
			e->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			e->GetTransform().SetOrientation(Matrix4::Rotation(180, Vector3(0, 1, 0)));
		}
		for (int i = 0; i < menuPlayers.size(); ++i) {
			GameObject* o = menuPlayers.at(i);
			if (i == 1)
				o->GetRenderObject()->SetColour({ 0.1,0.1,0.1,1 });
			o->GetTransform().SetOrientation(Matrix4::Rotation(180, Vector3(0, 1, 0)));
		}
		break;
	case 1:
		/* Player */
		InitPlayer();

		/* Moving state machine platforms*/
		platforms.push_back((PlatformStateGameObject*)AddFloorToWorld(
			new PlatformStateGameObject(Vector3(-12.5, 0, -49), Vector3(12.5, 0, -49)), Vector3(-12.5, 0, -49), { 12.5,1,24 }));
		platforms.push_back((PlatformStateGameObject*)AddFloorToWorld(
			new PlatformStateGameObject(Vector3(-12.5, 40, -130), Vector3(12.5, 40, -130)), Vector3(-12.5, 40, -130), { 5,1,5 }));
		platforms.push_back((PlatformStateGameObject*)AddFloorToWorld(
			new PlatformStateGameObject(Vector3(-45, -20, -990), Vector3(-45, 20, -990)), Vector3(-45, -20, -990), { 10,1,10 }));

		/* Bonus items */
		AddPickupToWorld(new PowerupObject, Vector3(0, 3, -20));
		AddPickupToWorld(new CoinObject, Vector3(-90, 3, -225));
		AddPickupToWorld(new CoinObject, Vector3(90, 3, -225));
		AddPickupToWorld(new CoinObject, Vector3(0, 3, -225));
		AddPickupToWorld(new CoinObject, Vector3(0, 25, -355));

		AddPickupToWorld(new CoinObject, Vector3(40, 3, -475))->GetTransform().SetOrientation(Matrix4::Rotation(-60, { 0, 1, 0 }));
		AddPickupToWorld(new CoinObject, Vector3(0, 3, -515))->GetTransform().SetOrientation(Matrix4::Rotation(-110, { 0, 1, 0 }));;;
		AddPickupToWorld(new CoinObject, Vector3(-40, 3, -555))->GetTransform().SetOrientation(Matrix4::Rotation(-60, { 0, 1, 0 }));

		AddPickupToWorld(new CoinObject, Vector3(-90, 3, -645));
		AddPickupToWorld(new CoinObject, Vector3(90, 3, -645));
		AddPickupToWorld(new CoinObject, Vector3(-90, 3, -825));
		AddPickupToWorld(new CoinObject, Vector3(90, 3, -825));
		AddPickupToWorld(new CoinObject, Vector3(-30, 3, -900))->GetTransform().SetOrientation(Matrix4::Rotation(90, { 0, 1, 0 }));
		AddPickupToWorld(new CoinObject, Vector3(-60, 3, -915));
		AddPickupToWorld(new PowerupObject, Vector3(-59, -13, -1055));
		AddPickupToWorld(new CoinObject, Vector3(-61, -13, -1057));
		AddPickupToWorld(new CoinObject, Vector3(-61, -13, -1053));
		AddPickupToWorld(new CoinObject, Vector3(-57, -13, -1053));
		AddPickupToWorld(new CoinObject, Vector3(-57, -13, -1057));

		AddPickupToWorld(new CoinObject, Vector3(-60, -14, -990));
		AddPickupToWorld(new CoinObject, Vector3(20, 20, -990))->GetTransform().SetOrientation(Matrix4::Rotation(270, { 0, 1, 0 }));

		/* Patrol enemy state machine */
		enemies.push_back((PatrolEnemyStateGameObject*)AddEnemyToWorld(new PatrolEnemyStateGameObject(positions), positions[0]));

		/* Behaviour tree enemy*/
		behaviourEnemies.push_back((BehaviourTreeEnemy*)AddEnemyToWorld(new BehaviourTreeEnemy(Vector3(0, 4, 0)), Vector3(0, 4, 0)));
		break;
	case 2:
		/* Player */
		InitPlayer();

		/* Pathfinding enemy state machien; one will not take costs into account, the others will, both using A* */
		int numEnemies = currentlySelected - 2;
		if (numEnemies >= 1)
			enemies.push_back((PathFindingEnemyStateGameObject*)AddEnemyToWorld(new PathFindingEnemyStateGameObject(true), Vector3(-5, 10, 0)));
		if (numEnemies >= 2)
			enemies.push_back((PathFindingEnemyStateGameObject*)AddEnemyToWorld(new PathFindingEnemyStateGameObject(false), Vector3(0, 10, 0)));
		if (numEnemies == 3)
			enemies.push_back((PathFindingEnemyStateGameObject*)AddEnemyToWorld(new PathFindingEnemyStateGameObject(false), Vector3(5, 10, 0)));

		break;
	}
}

/* Sets the locked and selected object to the player and enter third person mode */
void TutorialGame::InitPlayer() {
	Vector3 position = currentLevel == 1 ? Vector3(0, 10, 50) : Vector3(0, 15, 30);
	player = (PlayerObject*)AddPlayerToWorld(new PlayerObject, position);
	lockedObject = player;
	selectionObject = player;
	selectionObject->SetSelected(true);
	camState = CameraState::THIRDPERSON;
	world->GetMainCamera()->SetState(camState);
	world->GetMainCamera()->SetPosition(player->GetTransform().GetPosition());
}

/* This method will initialise any other moveable obstacles we want */
void TutorialGame::InitGameObstacles(int level) {
	switch (level) {
	case 1:
		/* Spinning Bar */
		/* Spinning Bar */
		AddCubeToWorld(new RotatingCubeObject, Vector3(0, 3, -735), Vector3(1, 1, 100));

		/* Spring Platforms*/
		AddCubeToWorld(new SpringObject(Vector3(0, 5, -890), 0.005f), Vector3(0, 5, -890), Vector3(4, 4, 1));
		AddCubeToWorld(new SpringObject(Vector3(-50, 5, -900), 0.005f), Vector3(-50, 5, -900), Vector3(1, 4, 4));
		AddCubeToWorld(new SpringObject(Vector3(-60, -15, -955), 0.01f), Vector3(-60, -15, -955), Vector3(10, 1, 15));

		/* Bridge */
		AddBridgeToWorld(Vector3(-30, 20, -990));
		break;
	}
}

/* Initialises a world with a number of sphere objects */
void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(new SphereObject, position, radius);
		}
	}
}

/* Initialises a world with a number of cube objects */
void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(new CubeObject, position, cubeDims);
		}
	}
}

/* Initialises a world with a number of random objects */
void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			int type = rand() % 3;
			switch (type) {
			case 0:
				(rand() % 2) ? AddCubeToWorld(new CubeObject, position, cubeDims * Vector3(1, 4, 1)) : AddCubeToWorld(new CubeObject, position, cubeDims);
				break;
			case 1:
				(rand() % 2) ? AddSphereToWorld(new SphereObject, position, sphereRadius / 2) : AddSphereToWorld(new SphereObject, position, sphereRadius);
				break;
			case 2:
				(rand() % 2) ? AddCapsuleToWorld(new CapsuleObject, position, 3.0f, sphereRadius * 1.5) : AddCapsuleToWorld(new CapsuleObject, position, 2.0f, sphereRadius);
				break;
			}
		}
	}
}

/* Adds a large immoveable cube to the world */
GameObject* TutorialGame::AddFloorToWorld(GameObject* floor, const Vector3& position, const Vector3& size, const Matrix4& orientation) {
	/* Checking if oriented (need OBB) */
	if (Vector3(1, 1, 1) != orientation * Vector3(1, 1, 1)) {
		OBBVolume* volume = new OBBVolume(size);
		floor->SetBoundingVolume((CollisionVolume*)volume);
	}
	else {
		AABBVolume* volume = new AABBVolume(size);
		floor->SetBoundingVolume((CollisionVolume*)volume);
	}

	floor->GetTransform().SetScale(size * 2).SetPosition(position).SetOrientation(orientation);
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, basicShader));

	if (dynamic_cast<LavaObject*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(lavaTex);
	else if (dynamic_cast<IceObject*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(iceTex);
	else if (dynamic_cast<TrampolineObject*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(trampolineTex);
	else if (dynamic_cast<FinishObject*>(floor))
		floor->GetRenderObject()->SetDefaultTexture(finishTex);

	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->GetPhysicsObject()->InitCubeInertia();
	world->AddGameObject(floor);
	return floor;
}

/* Builds a sphere object, both mesh and collision volume */
GameObject* TutorialGame::AddSphereToWorld(GameObject* sphere, const Vector3& position, float radius) {
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, obstacleTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetPhysicsObject()->InitSphereInertia(false);
	world->AddGameObject(sphere);
	return sphere;
}

/* Builds a cube object, depending on object type collision volume will be either AABB or OBB */
GameObject* TutorialGame::AddCubeToWorld(GameObject* cube, const Vector3& position, Vector3 dimensions) {
	CollisionVolume* volume;
	dynamic_cast<RotatingCubeObject*>(cube) ? volume = new OBBVolume(dimensions) : volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);
	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, obstacleTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->InitCubeInertia();
	world->AddGameObject(cube);
	return cube;
}

/* Builds a capsule object, both mesh and collision volume */
GameObject* TutorialGame::AddCapsuleToWorld(GameObject* capsule, const Vector3& position, float halfHeight, float radius) {
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);
	capsule->GetTransform().SetScale(Vector3(radius * 2, halfHeight, radius * 2)).SetPosition(position);
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, obstacleTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));
	capsule->GetPhysicsObject()->InitCapsuleInertia();
	world->AddGameObject(capsule);
	return capsule;
}

/* Adds a bridge to our world, held by constraints */
void TutorialGame::AddBridgeToWorld(Vector3 startPos) {
	Vector3 cubeSize(2.5, 0.5, 5);
	Vector3 baseSize(4, 1, 5);
	float bridgeWidth = 110;
	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 15;
	float maxDistance = (bridgeWidth / (numLinks + 2)); // constraint distance
	float cubeDistance = (bridgeWidth / (numLinks + 2)); // distance between links
	GameObject* start = AddFloorToWorld(new FloorObject, startPos + Vector3(0, 0, 0), baseSize);
	GameObject* end = AddFloorToWorld(new FloorObject, startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), baseSize);
	GameObject* previous = start;
	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(new CubeObject, startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize);
		block->GetRenderObject()->SetDefaultTexture(woodenTex);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

/* Add our player character with a capsule collision volume */
GameObject* TutorialGame::AddPlayerToWorld(GameObject* p, const Vector3& position) {
	float meshSize = 3.0f;
	CapsuleVolume* volume = new CapsuleVolume(meshSize * 0.85, 2.0f);
	p->SetBoundingVolume((CollisionVolume*)volume);
	p->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);
	(rand() % 2) ? p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshA, plainTex, basicShader)) :
		p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshB, plainTex, basicShader));
	p->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), p->GetBoundingVolume()));
	p->GetPhysicsObject()->InitCapsuleInertia();
	world->AddGameObject(p);
	return p;
}

/* Add an enemy character with a capsule collision volume */
GameObject* TutorialGame::AddEnemyToWorld(GameObject* e, const Vector3& position) {
	float meshSize = 3.0f;
	CapsuleVolume* volume = new CapsuleVolume(meshSize * 0.85, 2.0f);
	e->SetBoundingVolume((CollisionVolume*)volume);
	e->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);
	e->SetRenderObject(new RenderObject(&e->GetTransform(), enemyMesh, plainTex, basicShader));
	e->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	e->SetPhysicsObject(new PhysicsObject(&e->GetTransform(), e->GetBoundingVolume()));
	e->GetPhysicsObject()->InitCapsuleInertia();
	world->AddGameObject(e);
	return e;
}

/* Add a pickup item, either a coin or a boost, with a sphere collision volume */
GameObject* TutorialGame::AddPickupToWorld(GameObject* p, const Vector3& position) {
	SphereVolume* volume = new SphereVolume(1.25f);
	p->SetBoundingVolume((CollisionVolume*)volume);
	p->GetTransform().SetScale(Vector3(0.25, 0.25, 0.25)).SetPosition(position);
	p->SetRenderObject(new RenderObject(&p->GetTransform(), bonusMesh, basicTex, basicShader));
	dynamic_cast<PowerupObject*>(p) ? p->GetRenderObject()->SetColour(Debug::MAGENTA) : p->GetRenderObject()->SetColour(Debug::YELLOW);
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), p->GetBoundingVolume()));
	p->GetPhysicsObject()->InitSphereInertia(false);
	world->AddGameObject(p);
	return p;
}

/* If in debug mode we can select an object with the cursor, displaying its properties and allowing us to take control */
bool TutorialGame::SelectObject() {
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;

		if (world->Raycast(ray, closestCollision, nullptr, true)) {
			if (selectionObject) {
				/* Don't display where AI is going if not selected */
				if (dynamic_cast<EnemyStateGameObject*>(selectionObject))
					((EnemyStateGameObject*)selectionObject)->SetDisplayDirection(false);
				if (dynamic_cast<BehaviourTreeEnemy*>(selectionObject))
					((BehaviourTreeEnemy*)selectionObject)->SetDisplayRoute(false);
				selectionObject == player ? selectionObject->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1)) :
					selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject->SetSelected(false);
			}

			selectionObject = (GameObject*)closestCollision.node;
			/* Display AI directions */
			if (dynamic_cast<EnemyStateGameObject*>(selectionObject))
				((EnemyStateGameObject*)selectionObject)->SetDisplayDirection(true);
			if (dynamic_cast<BehaviourTreeEnemy*>(selectionObject))
				((BehaviourTreeEnemy*)selectionObject)->SetDisplayRoute(true);
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
			selectionObject->SetSelected(true);
			return true;
		}
		return false;
	}

	/* We can lock the object and move it around */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				camState = CameraState::FREE;
				lockedObject = nullptr;
			}
			else {
				camState = CameraState::THIRDPERSON;
				lockedObject = selectionObject;
			}
			world->GetMainCamera()->SetState(camState);
		}
	}
	return false;
}

/* If we've selected an object, we can manipulate it with some key presses */
void TutorialGame::DebugObjectMovement() {
	if (inSelectionMode && selectionObject) {
		/* Using the arrow keys and numpad we can twist the object with torque*/
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 0, -10));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 0, 10));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8))
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, nullptr, true)) {
				if (closestCollision.node == selectionObject)
					selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

/* If we have control of an object we can move it around and perform certain actions */
void TutorialGame::LockedObjectMovement(float dt) {
	if (inSelectionMode && selectionObject)
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	/* Forward is more tricky -  camera forward is 'into' the screen...so we can take a guess, and use
	the cross of straight up, and the right axis, to hopefully get a vector that's good enough! */
	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	Vector3 charForward = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	float force = 3.0f * dt;

	/* If controlling the player we can jump or move faster with bonuses */
	if (lockedObject == player) {
		renderer->DrawString("Jump[SPACE]", Vector2(84, 25), Debug::WHITE, textSize);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
			player->Jump();
		if (player->GetPowerUpTimer() > 0.0f) {
			force = 6.0f * dt;
			renderer->DrawString("Speed Boost:" + std::to_string((int)player->GetPowerUpTimer()), Vector2(40, 95), Debug::WHITE, textSize);
		}
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(fwdAxis * force);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(-rightAxis * force);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(-fwdAxis * force);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
		lockedObject->GetPhysicsObject()->ApplyLinearImpulse(rightAxis * force);

	/* We can lock the objects orientation with K or swap between camera positons with 1 */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K))
		lockedOrientation = !lockedOrientation;
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1)) {
		switch (camState) {
		case CameraState::THIRDPERSON:
			camState = CameraState::TOPDOWN;
			break;
		case CameraState::TOPDOWN:
			camState = CameraState::THIRDPERSON;
			break;
		}
		world->GetMainCamera()->SetState(camState);
	}
}
