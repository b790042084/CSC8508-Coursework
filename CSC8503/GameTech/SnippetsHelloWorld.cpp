//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2019 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

// ****************************************************************************
// This snippet illustrates simple use of physx
//
// It creates a number of box stacks on a plane, and if rendering, allows the
// user to create new stacks and fire a ball from the camera position
// ****************************************************************************

#include <ctype.h>

#include "PxPhysicsAPI.h"
#include "SnippetPrint.h"
#include "SnippetPVD.h"
#include <iostream>
#include "TutorialGame.h"

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxReal stackZ = 10.0f;

TutorialGame* tutorialGame;

void AddCubeToWorld(const PxTransform& t, PxVec3 fullExtents);
void AddSphereToWorld(const PxTransform& t, PxReal radius);
void AddCapsuleToWorld(const PxTransform& t, PxReal radius, PxReal halfHeight);
void AddPickupToWorld(const PxTransform& t, PxReal radius);
void AddPlayerToWorld(const PxTransform& t, float scale);
void AddEnemyToWorld(const PxTransform& t, float scale);
void AddFloorToWorld(const PxTransform& t, PxVec3 fullExtents);

void initPhysics() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
}

void stepPhysics(float dt) {
	gScene->simulate(dt);
	gScene->fetchResults(true);
}

void cleanupPhysics(bool /*interactive*/) {
	/*PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);*/
	if (gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	
		gPvd = NULL;
		//PX_RELEASE(transport);
	}
	//PX_RELEASE(gFoundation);

	printf("SnippetHelloWorld done.\n");
}

int snippetMain(int flag, const char* const*,TutorialGame* t, float dt ) {
#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	if (flag) {
		tutorialGame = t;
		initPhysics();
		AddSphereToWorld(PxTransform(PxVec3(-20, 50, -50)), 5);
		AddCubeToWorld(PxTransform(PxVec3(0, 50, -50)), PxVec3(10, 10, 10));
		AddCapsuleToWorld(PxTransform(PxVec3(20, 50, -50)), 5.0f, 10.0f);
		AddFloorToWorld(PxTransform(PxVec3(0, -20, 0)), PxVec3(1000.0f, 1, 1000));

		AddPickupToWorld(PxTransform(PxVec3(-20, 50, 0)), 3);
		AddPlayerToWorld(PxTransform(PxVec3(0, 50, 0)), 5);
		AddEnemyToWorld(PxTransform(PxVec3(20, 50, 0)), 5);

	}
	stepPhysics(.015f);
	//cleanupPhysics(false);
#endif

	return 0;
}

void AddCubeToWorld(const PxTransform& t, PxVec3 fullExtents) {
	GameObject* cube;
	PxRigidDynamic* body;
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(fullExtents.x / 2, fullExtents.y / 2, fullExtents.z / 2), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	body->setAngularVelocity(PxVec3(0.f, 0.f, 10.f));

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	cube = tutorialGame->AddPxCubeToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), Vector3(fullExtents.x / 2, fullExtents.y / 2, fullExtents.z / 2));
}

void AddSphereToWorld(const PxTransform& t, PxReal radius) {
	GameObject* sphere;
	PxRigidDynamic* body;
	PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	body->setAngularVelocity(PxVec3(0.f, 0.f, 10.f));

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	sphere = tutorialGame->AddPxSphereToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), radius);
}

void AddCapsuleToWorld(const PxTransform& t, PxReal radius, PxReal halfHeight) {
	GameObject* sphere;
	PxRigidDynamic* body;
	PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(radius, halfHeight), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	body->setAngularVelocity(PxVec3(0.f, 0.f, 10.f));

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	sphere = tutorialGame->AddPxCapsuleToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), radius, halfHeight);
}

void AddFloorToWorld(const PxTransform& t, PxVec3 fullExtents) {
	GameObject* floor;
	PxRigidStatic* body;
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(fullExtents.x / 2, fullExtents.y / 2, fullExtents.z / 2), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidStatic(t.transform(localTm));
	body->attachShape(*shape);

	gScene->addActor(*body);
	floor = tutorialGame->AddPxFloorToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), Vector3(fullExtents.x / 2, fullExtents.y / 2, fullExtents.z / 2));
}

void AddPickupToWorld(const PxTransform& t, PxReal radius) {
	GameObject* sphere;
	PxRigidStatic* body;
	PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidStatic(t.transform(localTm));
	body->attachShape(*shape);

	gScene->addActor(*body);
	sphere = tutorialGame->AddPxPickupToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), radius);
}

void AddPlayerToWorld(const PxTransform& t, float scale) {
	float meshSize = 3.0f * scale;
	GameObject* sphere;
	PxRigidDynamic* body;
	PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(meshSize * 0.66, meshSize * 0.85f), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	sphere = tutorialGame->AddPxPlayerToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), scale);
}

void AddEnemyToWorld(const PxTransform& t, float scale) {
	float meshSize = 3.0f * scale;
	GameObject* sphere;
	PxRigidDynamic* body;
	PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(meshSize * 0.66, meshSize * 0.85f), *gMaterial);
	PxTransform localTm(t.p);
	body = gPhysics->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	sphere = tutorialGame->AddPxEnemyToWorld(new GameObject(), body, Vector3(t.p.x, t.p.y, t.p.z), scale);
}