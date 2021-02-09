/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *		    	AABB Definition		 */
#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
namespace NCL {
	class AABBVolume : public CollisionVolume {
	public:
		AABBVolume(const Vector3& halfDims) {
			type = VolumeType::AABB;
			halfSizes = halfDims;
		}
		~AABBVolume() {

		}

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

	protected:
		Vector3 halfSizes;
	};
}
