/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		    	Capsule Definition		 */
#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class CapsuleVolume : public CollisionVolume {
	public:
		CapsuleVolume(float halfHeight, float radius) {
			if (radius >= halfHeight)
				radius = halfHeight - 0.5f;  // Don't want capsules with greater radius than half height
			this->halfHeight = halfHeight;
			this->radius = radius;
			this->type = VolumeType::Capsule;
		};
		~CapsuleVolume() {

		}
		float GetRadius() const {
			return radius;
		}

		float GetHalfHeight() const {
			return halfHeight;
		}

	protected:
		float radius;
		float halfHeight;
	};
}

