/*		 Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Rotating Cube Object Definition		 */
#pragma once
#include "CubeObject.h"
/* This class is largely here to identify why cubes I want to rotate in the physics engine */
namespace NCL {
	namespace CSC8503 {
		class RotatingCubeObject : public CubeObject {
		public:
			RotatingCubeObject() {
				invMass = 0.0f;		// No mass
				name = "RotatingCube";
			}
		};
	}
}