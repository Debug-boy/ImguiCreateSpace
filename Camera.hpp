#pragma once
#include "Vector2.h"
#include "Rotation.hpp"
#include "Translation.hpp"

namespace infinity {
#define		REF(TYPE)	TYPE&
#define		C_REF(TYPE) const TYPE&

	class Camera {
	private:
		Rotation m_rotation{};
		Translation m_translation{};
		Matrix3x3 m_rotateRollMatrix{};
		Matrix3x3 m_rotatePitchMatrix{};
		Matrix3x3 m_rotateYawMatrix{};
		Matrix3x3 m_rotationUnitMatrix{};
		Vector2 m_worldScreenSize{};
		Vector2 m_worldSight{};

	private:
		void upDataMatrix() {
			m_rotation.getRollMatrix(m_rotateRollMatrix);
			m_rotation.getPitchMatrix(m_rotatePitchMatrix);
			m_rotation.getYawMatrix(m_rotateYawMatrix);
			m_rotationUnitMatrix = m_rotateRollMatrix * m_rotatePitchMatrix * m_rotateYawMatrix;
		}

		void updtatSight() {
			m_worldSight.x = m_worldScreenSize.x * 0.5f;
			m_worldSight.y = m_worldScreenSize.y * 0.5f;
		}

	public:
		Camera(){}

		Camera(const Rotation& rotation, const Translation& translation, const Vector2& worldScreenSize) {
			m_rotation = rotation;
			m_translation = translation;
			m_worldScreenSize = worldScreenSize;
			m_worldSight = Vector2(worldScreenSize.x / 2, worldScreenSize.y / 2);
		}

		~Camera() = default;

		void setRotation(const float &roll,const float &pitch,const float &yaw) {
			m_rotation.set(roll, pitch, yaw);
			upDataMatrix();
		}

		void setRotation(const Rotation& rotation) {
			m_rotation.set(rotation);
			upDataMatrix();
		}

		void setTranslation(const float& x, const float& y, const float& z) {
			m_translation.reset(x, y, z);
		}

		void setTranslation(const Translation &tanslation) {
			m_translation.set(tanslation);
		}

		void setTranslation(float rate) {
			m_translation *= rate;
		}
		
		void setWorldScreenSize(const Vector2 &worldScreenSize) {
			m_worldScreenSize = worldScreenSize;
			updtatSight();
		}

		Matrix3x3 getRollMatrix() const{
			return m_rotateRollMatrix;
		}

		Matrix3x3 getPitchMatrix() const {
			return m_rotatePitchMatrix;
		}

		bool worldToScreen(C_REF(Vector3) worldLocation, REF(Vector2) screenPosition) {
			return WorldToScreen(m_rotationUnitMatrix, m_translation, m_worldSight, worldLocation, screenPosition);
		}

		static bool WorldToScreen(C_REF(Matrix3x3) cameraRotate, C_REF(Translation) cameraTanslate, C_REF(Vector2) sight, C_REF(Vector3) worldLocation, REF(Vector2) screenPosition) {
			float viewZ = 
				(cameraRotate.m31 * worldLocation.x) + 
				(cameraRotate.m32 * worldLocation.y) + 
				(cameraRotate.m33 * worldLocation.z) + cameraTanslate.z;

			viewZ = 1.0f / viewZ;

			screenPosition.x = sight.x + (
				(cameraRotate.m11 * worldLocation.x) +
				(cameraRotate.m12 * worldLocation.y) +
				(cameraRotate.m13 * worldLocation.z) + cameraTanslate.x) * sight.x * viewZ;

			screenPosition.y = sight.y - (
				(cameraRotate.m21 * worldLocation.x) +
				(cameraRotate.m22 * worldLocation.y) +
				(cameraRotate.m23 * worldLocation.z) + cameraTanslate.y) * sight.y * viewZ;
			return true;
		}

	};

}