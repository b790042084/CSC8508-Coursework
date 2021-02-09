/*		 Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Lava Object Definition		 */
#pragma once
#include "FloorObject.h"
namespace NCL {
	namespace CSC8503 {
		class IceObject : public FloorObject {
		public:
			IceObject() {
				friction = 0.0f;		// No friction on ice tiles
				name = "Ice";
			}
		};
	}
}