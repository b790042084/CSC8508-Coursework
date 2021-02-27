/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Physics Object Implementation		 */
#include "PhysicsObject.h"
#include "PhysicsSystem.h"
#include "../CSC8503Common/Transform.h"
using namespace NCL;
using namespace CSC8503;

PhysicsObject::PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume) {
	transform = parentTransform;
	volume = parentVolume;
	inverseMass = 1.0f;
	elasticity = 0.8f;
	friction = 0.8f;
	isAsleep = true;
	isStatic = false;
}

PhysicsObject::PhysicsObject(Transform* parentTransform, PxRigidActor* p, const CollisionVolume* parentVolume)
{
	transform = parentTransform;
	pxTrans = p;
	volume = parentVolume;
	inverseMass = 1.0f;
	elasticity = 0.8f;
	friction = 0.8f;
	isAsleep = true;
	isStatic = false;
}

PhysicsObject::~PhysicsObject() {

}

void PhysicsObject::ApplyAngularImpulse(const Vector3& force) {
	if (force.Length() > 0) {
		bool a = true;
	}
	angularVelocity += inverseInteriaTensor * force;
}

void PhysicsObject::ApplyLinearImpulse(const Vector3& force) {
	linearVelocity += force * inverseMass;
}

void PhysicsObject::AddForce(const Vector3& addedForce) {
	force += addedForce;
}

void PhysicsObject::AddForceAtPosition(const Vector3& addedForce, const Vector3& position) {
	Vector3 localPos = position - transform->GetPosition();
	force += addedForce;
	torque += Vector3::Cross(localPos, addedForce);
}

void PhysicsObject::AddForceAtLocalPosition(const Vector3& addedForce, const Vector3& position) {
	force += addedForce;
	torque += Vector3::Cross(position, addedForce);
}

void PhysicsObject::AddTorque(const Vector3& addedTorque) {
	torque += addedTorque;
}

void PhysicsObject::ClearForces() {
	force = Vector3();
	torque = Vector3();
}

void PhysicsObject::InitCubeInertia() {
	Vector3 dimensions = transform->GetScale();

	Vector3 fullWidth = dimensions * 2;

	Vector3 dimsSqr = fullWidth * fullWidth;

	inverseInertia.x = (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
	inverseInertia.y = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
	inverseInertia.z = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
}

void PhysicsObject::InitSphereInertia(bool hollow) {
	float k = hollow ? 1.5f : 2.5f;		// We have different inertias for hollow spheres vs solid ones
	float radius = transform->GetScale().GetMaxElement();
	float i = k * inverseMass / (radius * radius);

	inverseInertia = Vector3(i, i, i);
}

/* i ~= mr ^ 2 */
void PhysicsObject::InitCapsuleInertia() {
	float radius = transform->GetScale().GetMaxElement();
	float i = inverseMass / (radius * radius);

	inverseInertia = Vector3(i, i, i);
}

void PhysicsObject::UpdateInertiaTensor() {
	Quaternion q = transform->GetOrientation();

	Matrix3 invOrientation = Matrix3(q.Conjugate());
	Matrix3 orientation = Matrix3(q);

	inverseInteriaTensor = orientation * Matrix3::Scale(inverseInertia) * invOrientation;
}