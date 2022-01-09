#pragma once
#include "Matrix3x3.hpp"

namespace infinity {

	class Rotation {
	private:
		float m_roll;
		float m_pitch;
		float m_yaw;

	public:
		Rotation() :m_roll(0.0f), m_pitch(0.0f), m_yaw(0.0f) {}

		Rotation(float roll, float picth, float yaw) {
			m_roll = roll;
			m_pitch = picth;
			m_yaw = yaw;
		}

		float getRoll() const {
			return m_roll;
		}

		float getPitch() const {
			return m_pitch;
		}

		float getYaw() const {
			return m_yaw;
		}

		static void getRollMatrix(Matrix3x3& matrix,const float &roll) {
			matrix.m11 = 1.0f;
			matrix.m12 = 0.0f;
			matrix.m13 = 0.0f;

			matrix.m21 = 0.0f;
			matrix.m22 = cosf(roll);
			matrix.m23 = sinf(roll);

			matrix.m31 = 0.0f;
			matrix.m32 = -sinf(roll);
			matrix.m33 = cosf(roll);
		}

		void getRollMatrix(Matrix3x3& matrix) const{
			getRollMatrix(matrix, m_roll);
		}

		static void getPitchMatrix(Matrix3x3 &matrix,const float &pitch) {
			matrix.m11 = cosf(pitch);
			matrix.m12 = 0.0f;
			matrix.m13 = -sinf(pitch);

			matrix.m21 = 0.0f;
			matrix.m22 = 1.0f;
			matrix.m23 = 0.0f;

			matrix.m31 = sinf(pitch);
			matrix.m32 = 0.0f;
			matrix.m33 = cosf(pitch);
		}

		void getPitchMatrix(Matrix3x3& matrix) const{
			getPitchMatrix(matrix, m_pitch);
		}

		static void getYawMatrix(Matrix3x3& matrix,const float &yaw) {
			matrix.m11 = cosf(yaw);
			matrix.m12 = sinf(yaw);
			matrix.m13 = 0.0f;

			matrix.m21 = -sinf(yaw);
			matrix.m22 = cosf(yaw);
			matrix.m23 = 0.0f;

			matrix.m31 = 0.0f;
			matrix.m32 = 0.0f;
			matrix.m33 = 1.0f;
		}

		void getYawMatrix(Matrix3x3& matrix) const{
			getYawMatrix(matrix, m_yaw);
		}

		void rotateToWorld(const Vector3& vector,Vector3& out) {
			Matrix3x3 matrix_x, matrix_y, mstrix_z;
			getRollMatrix(matrix_x);
			getPitchMatrix(matrix_y);
			getYawMatrix(mstrix_z);
			out = ((matrix_x * matrix_y * mstrix_z)) * vector;
		}

		void setRoll(const float& roll) {
			m_roll = roll;
		}

		void setPitch(const float& pitch) {
			m_pitch = pitch;
		}

		void setYaw(const float& yaw) {
			m_yaw = yaw;
		}

		void setRotateRate(const float& rate) {
			m_roll *= rate;
			m_pitch *= rate;
			m_yaw *= rate;
		}

		void set(const float& roll, const float& pitch, const float& yaw) {
			m_roll = roll;
			m_pitch = pitch;
			m_yaw = yaw;
		}

		void set(const Rotation &rotation) {
			set(rotation.getRoll(), rotation.getPitch(), rotation.getYaw());
		}
	};
}

