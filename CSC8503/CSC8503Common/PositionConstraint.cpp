/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Position Constraint Implementation		 */
#include "PositionConstraint.h"
using namespace NCL;
using namespace CSC8503;

void PositionConstraint::UpdateConstraint(float dt) {
	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	float currentDistance = relativePos.Length();
	float offset = distance - currentDistance;
	if (abs(offset) > 0.0f) {
		Vector3 offsetDir = relativePos.Normalised();
		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 angVelocityA = Vector3::Cross(physA->GetAngularVelocity(), relativePos);
		Vector3 angVelocityB = Vector3::Cross(physB->GetAngularVelocity(), -relativePos);

		Vector3 fullVelocityA = physA->GetLinearVelocity() + angVelocityA;
		Vector3 fullVelocityB = physB->GetLinearVelocity() + angVelocityB;

		Vector3 relativeVelocity = fullVelocityA - fullVelocityB;

		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			// how much of their relative force is affecting the constraint
			float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);

			/* Get our angular effects */
			Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativePos, offsetDir), relativePos);
			Vector3 inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(-relativePos, offsetDir), -relativePos);
			float angularEffect = Vector3::Dot(inertiaA + inertiaB, offsetDir);

			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset;
			float lambda = -(velocityDot + bias) / (constraintMass + angularEffect);

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyLinearImpulse(aImpulse); // multiplied by mass here
			physA->ApplyAngularImpulse(Vector3::Cross(relativePos, aImpulse));
			physB->ApplyLinearImpulse(bImpulse); // multiplied by mass here
			physB->ApplyAngularImpulse(Vector3::Cross(-relativePos, bImpulse));
		}
	}
}
