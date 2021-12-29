#pragma once
#include "Vector3.hpp"

namespace infinity {
	class Translation : public Vector3 {
	public:
		Translation() {}
		Translation(float x, float y, float z) :Vector3(x, y, z) {}
		~Translation() = default;

		virtual Translation operator * (const float& rate) {
			return Translation(this->x *= rate, this->y *= rate, this->z *= rate);
		}

		virtual void operator *= (const float& rate) {
			this->x *= rate;
			this->y *= rate;
			this->z *= rate;
		}
	};
}