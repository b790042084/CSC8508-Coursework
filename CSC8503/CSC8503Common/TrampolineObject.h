/*		 Created By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Trampoline Object Definition		 */
#pragma once
#include "FloorObject.h"

namespace NCL {
	namespace CSC8503 {
		class TrampolineObject : public FloorObject {
		public:
			TrampolineObject() {
				elasticity = 8.0f;		// High elasticity to gain kinetic energy
				name = "Trampoline";
			}
		};
	}
}