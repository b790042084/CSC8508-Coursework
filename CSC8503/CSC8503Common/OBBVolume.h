/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			OBB Volume Definition		 */
#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
namespace NCL {
	class OBBVolume : public CollisionVolume {
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type = VolumeType::OBB;
			halfSizes = halfDims;
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const {
			return halfSizes;
		}
	protected:
		Maths::Vector3 halfSizes;
	};
}

