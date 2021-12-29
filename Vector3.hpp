#pragma once

namespace infinity {
	class Vector3 {
	public:
		float x;
		float y;
		float z;

	public:

		Vector3() :x(), y(), z() {}

		Vector3(float _x, float _y, float _z) {
			this->x = _x;
			this->y = _y;
			this->z = _z;
		}

		~Vector3() = default;

	public:
		void reset(float reX, float reY, float reZ) {
			this->x = reX;
			this->y = reY;
			this->z = reZ;
		}

		void set(const Vector3& vector) {
			this->x = vector.x;
			this->y = vector.y;
			this->z = vector.z;
		}

		Vector3 operator + (const Vector3& vector) {
			return Vector3(this->x + vector.x, this->y + vector.y, this->z + vector.z);
		}

		Vector3 operator - (const Vector3& vector) {
			return Vector3(this->x - vector.x, this->y - vector.y, this->z - vector.z);
		}

		virtual float operator * (const Vector3& vector) {
			return (this->x * vector.x) + (this->y * vector.y) + (this->z * vector.z);
		}

	};
}