/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Physics System Implementation		 */
#include "PhysicsSystem.h"
using namespace NCL;
using namespace CSC8503;

/*
These two variables help define the relationship between positions
and the forces that are added to objects to change those positions
*/
PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g) {
	applyGravity = false;
	useBroadPhase = true;
	dTOffset = 0.0f;
	globalDamping = 0.995f;
	dampingFactor = 0.4;
	SetGravity(Vector3(0.0f, -9.8f, 0.0f));
	basicCollisionsTested = 0;
	totalCollisions = 0;
}

PhysicsSystem::~PhysicsSystem() {
}

/* If the 'game' is ever reset, the PhysicsSystem must be
'cleared' to remove any old collisions that might still
be hanging around in the collision list. If your engine
is expanded to allow objects to be removed from the world,
you'll need to iterate through this collisions list to remove
any collisions they are in. */
void PhysicsSystem::Clear() {
	allCollisions.clear();
}

void PhysicsSystem::ClearDeletedCollisions() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end();) {
		if ((!i->a->IsActive() && !i->a->GetSelected()) || (!i->b->IsActive() && !i->b->GetSelected())) {
			/* We need to remove objects from our collision list before we consider deleting them, so let's do it */
			if (!i->a->IsActive())
				i->a->SetIsSafeForDeletion(true);
			if (!i->b->IsActive())
				i->b->SetIsSafeForDeletion(true);
			i = allCollisions.erase(i);
		}
		else
			++i;
	}
}

//This is the fixed timestep we'd LIKE to have
const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;

/* This is the fixed update we actually have...
If physics takes too long it starts to kill the framerate, it'll drop the
iteration count down until the FPS stabilises, even if that ends up
being at a low rate. */
int realHZ = idealHZ;
float realDT = idealDT;

void PhysicsSystem::Update(float dt) {

	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	GameTimer t;
	t.GetTimeDeltaSeconds();

	if (useBroadPhase) {
		UpdateObjectAABBs();
	}

	while (dTOffset >= realDT) {
		IntegrateAccel(realDT); //Update accelerations from external forces
		if (useBroadPhase) {
			BroadPhase();
			NarrowPhase();
		}
		else
			BasicCollisionDetection();

		/* This is our simple iterative solver -
		  we just run things multiple times, slowly moving things forward
		 and then rechecking that the constraints have been met */
		float constraintDt = realDT / (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i) {
			UpdateConstraints(constraintDt);
		}
		IntegrateVelocity(realDT); //update positions from new velocity changes

		dTOffset -= realDT;
	}

	ClearForces();	//Once we've finished with the forces, reset them to zero

	UpdateCollisionList(); //Remove any old collisions

	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();

	//Uh oh, physics is taking too long...
	if (updateTime > realDT) {
		realHZ /= 2;
		realDT *= 2;
		std::cout << "Dropping iteration count due to long physics time...(now " << realHZ << ")\n";
	}
	else if (dt * 2 < realDT) { //we have plenty of room to increase iteration count!
		int temp = realHZ;
		realHZ *= 2;
		realDT /= 2;

		if (realHZ > idealHZ) {
			realHZ = idealHZ;
			realDT = idealDT;
		}
		if (temp != realHZ) {
			std::cout << "Raising iteration count due to short physics time...(now " << realHZ << ")\n";
		}
	}
}

/* Later on we're going to need to keep track of collisions
across multiple frames, so we store them in a set.

The first time they are added, we tell the objects they are colliding.
The frame they are to be removed, we tell them they're no longer colliding.

From this simple mechanism, we we build up gameplay interactions inside the
OnCollisionBegin / OnCollisionEnd functions (removing health when hit by a
rocket launcher, gaining a point when the player hits the gold coin, and so on). */
void PhysicsSystem::UpdateCollisionList() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); ) {
		if ((*i).framesLeft == numCollisionFrames) {
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}
		(*i).framesLeft = (*i).framesLeft - 1;
		if ((*i).framesLeft < 0) {
			i->a->OnCollisionEnd(i->b);
			i->b->OnCollisionEnd(i->a);
			i = allCollisions.erase(i);
		}
		else {
			++i;
		}
	}
}

void PhysicsSystem::UpdateObjectAABBs() {
	gameWorld.OperateOnContents([](GameObject* g) {
		g->UpdateBroadphaseAABB();
		}
	);
}

/* This is how we'll be doing collision detection in tutorial 4.
We step thorugh every pair of objects once (the inner for loop offset
ensures this), and determine whether they collide, and if so, add them
to the collision set for later processing. The set will guarantee that
a particular pair will only be added once, so objects colliding for
multiple frames won't flood the set with duplicates. */
void PhysicsSystem::BasicCollisionDetection() {
	basicCollisionsTested = 0;
	totalCollisions = 0;
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		if ((*i)->GetPhysicsObject() == nullptr) {
			continue;
		}
		for (auto j = i + 1; j != last; ++j) {
			if ((*j)->GetPhysicsObject() == nullptr) {
				continue;
			}
			CollisionDetection::CollisionInfo info;
			basicCollisionsTested++;
			if (CollisionDetection::ObjectIntersection(*i, *j, info)) {
				if (!dynamic_cast<PickupObject*>(info.a) && !dynamic_cast<PickupObject*>(info.b))	// We dont want to resolve collisions with pickup volume
					ImpulseResolveCollision(*info.a, *info.b, info.point);
				info.framesLeft = numCollisionFrames;
				allCollisions.insert(info);
			}
		}
	}
	totalCollisions = allCollisions.size();
}

/* In tutorial 5, we start determining the correct response to a collision,
so that objects separate back out. */
void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {
	PhysicsObject* physA = a.GetPhysicsObject();
	PhysicsObject* physB = b.GetPhysicsObject();
	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();
	float totalMass = physA->GetInverseMass() + physB->GetInverseMass();
	if (totalMass == 0) {
		return; // two static objects ??
	}
	/* Separate Using Projection (not on infinite mass objects) */
	if (physA->GetInverseMass() != 0.0f)
		transformA.SetPosition(transformA.GetPosition() - (p.normal * p.penetration * (physA->GetInverseMass() / totalMass)));
	if (physB->GetInverseMass() != 0.0f)
		transformB.SetPosition(transformB.GetPosition() + (p.normal * p.penetration * (physB->GetInverseMass() / totalMass)));

	/* Separate Using Impulse */
	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;
	Vector3 angVelocityA = Vector3::Cross(physA->GetAngularVelocity(), relativeA);
	Vector3 angVelocityB = Vector3::Cross(physB->GetAngularVelocity(), relativeB);
	Vector3 fullVelocityA = physA->GetLinearVelocity() + angVelocityA;
	Vector3 fullVelocityB = physB->GetLinearVelocity() + angVelocityB;
	Vector3 contactVelocity = fullVelocityB - fullVelocityA;
	float impulseForce = Vector3::Dot(contactVelocity, p.normal);
	// now to work out the effect of inertia ....
	Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, p.normal), relativeA);
	Vector3 inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, p.normal), relativeB);
	float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);
	float cRestitution = physA->GetElasticity() * physB->GetElasticity(); // disperse some kinectic energy
	float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);
	Vector3 fullImpulse = p.normal * j;
	/* Dont apply impulse to our infinite mass objects */
	if (physA->GetInverseMass() != 0.0f) {
		physA->ApplyLinearImpulse(-fullImpulse);
		physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fullImpulse));
	}
	if (physB->GetInverseMass() != 0.0f) {
		physB->ApplyLinearImpulse(fullImpulse);
		physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fullImpulse));
	}

	/* Separate Using Friction */
	Vector3 t = contactVelocity - (p.normal * impulseForce);
	t.Normalise();
	float fricForce = Vector3::Dot(contactVelocity, t);
	Vector3 fricInertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, t), relativeA);
	Vector3 fricInertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, t), relativeB);
	float fricEffect = Vector3::Dot(fricInertiaA + fricInertiaB, t);
	float fricCoef = std::clamp(physA->GetFriction() * physB->GetFriction(), 0.0f, 1.0f);
	float jT = -(fricCoef * fricForce) / (totalMass + fricEffect);
	Vector3 fricImpulse = t * jT;
	/* Frictional effects */
	if (physA->GetInverseMass() != 0.0f) {
		physA->ApplyLinearImpulse(-fricImpulse);
		physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fricImpulse));
	}
	if (physB->GetInverseMass() != 0.0f) {
		physB->ApplyLinearImpulse(fricImpulse);
		physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fricImpulse));
	}

	/* Separate Using Penalty */
	float k = 0.005;
	float extension = 0 - p.penetration;
	float force = -k * extension;
	if (physA->GetInverseMass() != 0.0f)
		physA->AddForceAtLocalPosition(p.normal * -force, p.localA);
	if (physB->GetInverseMass() != 0.0f)
		physB->AddForceAtLocalPosition(p.normal * force, p.localB);
}

/* Directly apply spring forces to move an object to its rest position */
void PhysicsSystem::SpringTowardsPoint(SpringObject* a) const {
	Vector3 extension = a->GetTransform().GetPosition() - a->GetRestPosition();
	Vector3 force = extension * -a->GetK();
	a->GetPhysicsObject()->ApplyLinearImpulse(force);
}

/* Later, we replace the BasicCollisionDetection method with a broadphase
and a narrowphase collision detection method. In the broad phase, we
split the world up using an acceleration structure, so that we can only
compare the collisions that we absolutely need to. */
void PhysicsSystem::BroadPhase() {
	broadphaseCollisions.clear();
	QuadTree<GameObject*> tree(Vector2(2048, 2048), 7, 6);
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes)) {
			continue;
		}
		Vector3 pos = (*i)->GetTransform().GetPosition();

		tree.Insert(*i, pos, halfSizes);
	}
	tree.OperateOnContents([&](std::list<QuadTreeEntry<GameObject*>>& data) {
		CollisionDetection::CollisionInfo info;
		for (auto i = data.begin(); i != data.end(); ++i) {
			for (auto j = std::next(i); j != data.end(); ++j) {
				info.a = min((*i).object, (*j).object);
				info.b = max((*i).object, (*j).object);

				/* Setting our sleeping and static objects */
				if (info.a->GetPhysicsObject()->GetInverseMass() == 0.0f)
					info.a->GetPhysicsObject()->SetIsStatic(true);
				else
					info.a->GetPhysicsObject()->SetIsStatic(false);
				if (info.b->GetPhysicsObject()->GetInverseMass() == 0.0f)
					info.b->GetPhysicsObject()->SetIsStatic(true);
				else
					info.b->GetPhysicsObject()->SetIsStatic(false);

				if (info.a->GetPhysicsObject()->GetLinearVelocity().Length() == 0.0f && info.a->GetPhysicsObject()->GetAngularVelocity().Length() == 0.0f)
					info.a->GetPhysicsObject()->SetIsAsleep(true);
				else
					info.a->GetPhysicsObject()->SetIsAsleep(false);
				if (info.b->GetPhysicsObject()->GetLinearVelocity().Length() == 0.0f && info.b->GetPhysicsObject()->GetAngularVelocity().Length() == 0.0f)
					info.b->GetPhysicsObject()->SetIsAsleep(true);
				else
					info.b->GetPhysicsObject()->SetIsAsleep(false);

				bool bothStatic = info.a->GetPhysicsObject()->GetIsStatic() && info.b->GetPhysicsObject()->GetIsStatic();
				bool bothAsleep = info.a->GetPhysicsObject()->GetIsAsleep() && info.b->GetPhysicsObject()->GetIsAsleep();

				/* If both object are asleep or static, dont test */
				if (!(bothStatic || bothAsleep))
					broadphaseCollisions.insert(info);
			}
		}
		});
}

/* The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list */
void PhysicsSystem::NarrowPhase() {
	totalCollisions = 0;
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i) {
		CollisionDetection::CollisionInfo info = *i;
		if (CollisionDetection::ObjectIntersection(info.a, info.b, info)) {
			info.framesLeft = numCollisionFrames;
			if (!dynamic_cast<PickupObject*>(info.a) && !dynamic_cast<PickupObject*>(info.b))		// Don't resolve collisions with pickup objects
				ImpulseResolveCollision(*info.a, *info.b, info.point);
			allCollisions.insert(info);
			totalCollisions++;
		}
	}
	totalCollisions = allCollisions.size();
}

/* Integration of acceleration and velocity is split up, so that we can
move objects multiple times during the course of a PhysicsUpdate,
without worrying about repeated forces accumulating etc.

This function will update both linear and angular acceleration,
based on any forces that have been accumulated in the objects during
the course of the previous game frame. */
void PhysicsSystem::IntegrateAccel(float dt) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		PhysicsObject* object = (*i)->GetPhysicsObject();
		if (object == nullptr) {
			continue; // No physics object for this GameObject !
		}
		float inverseMass = object->GetInverseMass();
		Vector3 linearVel = object->GetLinearVelocity();
		Vector3 force = object->GetForce();
		Vector3 accel = force * inverseMass;
		if (applyGravity && inverseMass > 0) {
			accel += gravity * 3; // don ’t move infinitely heavy things
		}
		linearVel += accel * dt; // integrate accel !
		object->SetLinearVelocity(linearVel);

		// Angular stuff
		Vector3 torque = object->GetTorque();
		Vector3 angVel = object->GetAngularVelocity();
		object->UpdateInertiaTensor(); // update tensor vs orientation

		Vector3 angAccel = object->GetInertiaTensor() * torque;
		angVel += angAccel * dt; // integrate angular accel !
		object->SetAngularVelocity(angVel);
	}
}

/* This function integrates linear and angular velocity into
position and orientation. It may be called multiple times
throughout a physics update, to slowly move the objects through
the world, looking for collisions. */
void PhysicsSystem::IntegrateVelocity(float dt) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	float frameLinearDamping = 1.0f - (dampingFactor * dt);
	for (auto i = first; i != last; ++i) {
		PhysicsObject* object = (*i)->GetPhysicsObject();
		if (object == nullptr) {
			continue;
		}
		Transform& transform = (*i)->GetTransform();
		// Position Stuff
		Vector3 position = transform.GetPosition();
		Vector3 linearVel = object->GetLinearVelocity();
		position += linearVel * dt;
		transform.SetPosition(position);
		// Linear Damping
		linearVel = linearVel * frameLinearDamping;
		object->SetLinearVelocity(linearVel);

		// Orientation Stuff
		Quaternion orientation = transform.GetOrientation();
		Vector3 angVel = dynamic_cast<RotatingCubeObject*>((*i)) ? Vector3(0, 1, 0) : object->GetAngularVelocity();
		orientation = orientation + (Quaternion(angVel * dt * 0.5f, 0.0f) * orientation);
		orientation.Normalise();
		transform.SetOrientation(orientation);

		// Damp the angular velocity too
		float frameAngularDamping = 1.0f - (dampingFactor * dt);
		angVel = angVel * frameAngularDamping;
		object->SetAngularVelocity(angVel);

		/* Spring objects should have some force added towards their resting position */
		if (dynamic_cast<SpringObject*>((*i)))
			SpringTowardsPoint((SpringObject*)(*i));
	}
}

/* Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame. */
void PhysicsSystem::ClearForces() {
	gameWorld.OperateOnContents(
		[](GameObject* o) {
			o->GetPhysicsObject()->ClearForces();
		}
	);
}

/* As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc. */
void PhysicsSystem::UpdateConstraints(float dt) {
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);
	for (auto i = first; i != last; ++i) {
		(*i)->UpdateConstraint(dt);
	}
}