/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Sphere Volume Definition		 */
#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class SphereVolume : public CollisionVolume {
	public:
		SphereVolume(float sphereRadius = 1.0f) {
			type = VolumeType::Sphere;
			radius = sphereRadius;
		}
		~SphereVolume() {}

		float GetRadius() const {
			return radius;
		}
	protected:
		float	radius;
	};
}

