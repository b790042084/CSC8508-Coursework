/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game World Implementation		 */
#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm>
#include <random>
#include "PlayerObject.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete)
		delete o;
}

void GameWorld::GetObjectIterators(GameObjectIterator& first, GameObjectIterator& last) const {
	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	std::random_device rd;
	std::mt19937 g(rd());
	if (shuffleObjects) 
		std::shuffle(gameObjects.begin(), gameObjects.end(), g);
	if (shuffleConstraints) 
		std::shuffle(constraints.begin(), constraints.end(), g);
	for (auto& i : gameObjects) {
		i->Update();
		if (i->GetTimeAlive() > 40.0f)		// Objects living longer than 40s are destroyed
			i->SetIsActive(false);
		if (i->GetIsSafeForDeletion())		// Only when objects have been removed from any associated collision list, can we delete them
			RemoveGameObject(i, true);
	}
}

void GameWorld::ShowFacing() {
	for (auto& i : gameObjects) {
		if(i->IsActive())
			Debug::DrawAxisLines(i->GetTransform().GetMatrix(), 2.0f);		// Show the axes of all active game objects
	}
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, GameObject* current, bool closestObject) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;
	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision) && i != current) {
			if (!closestObject) {	
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}

/* Constraint Tutorial Stuff */
void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(std::vector<Constraint*>::const_iterator& first, std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}


