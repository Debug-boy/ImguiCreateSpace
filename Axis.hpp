#pragma once

namespace infinity {

	class AxisSpace {
	
	private:
		float m_scaleX;
		float m_scaleY;
		float m_scaleZ;

		Vector3 negative_location_x, positive_location_x;
		Vector3 negative_location_y, positive_location_y;
		Vector3 negative_location_z, positive_location_z;

	public:
		AxisSpace() :m_scaleX(), m_scaleY(), m_scaleZ() {}

		AxisSpace(float scale_x, float scale_y, float scale_z) {
			m_scaleX = scale_x;
			m_scaleY = scale_y;
			m_scaleZ = scale_z;
		}

		~AxisSpace() {  }

	public :
		
		float getScaleX() const {
			return m_scaleX;
		}

		float getScaleY() const {
			return m_scaleY;
		}

		float getScaleZ() const {
			return m_scaleZ;
		}

		void setScaleX(const float &scale_x) {
			m_scaleX = scale_x;
			negative_location_x.reset(-m_scaleX, 0.0f, 0.0f);
			positive_location_x.reset(m_scaleX, 0.0f, 0.0f);
		}

		void setScaleY(const float& scale_y) {
			m_scaleY = scale_y;
			negative_location_y.reset(0.0f, -m_scaleY, 0.0f);
			positive_location_y.reset(0.0f, m_scaleY, 0.0f);
		}

		void setScaleZ(const float& scale_z) {
			m_scaleZ = scale_z;
			negative_location_z.reset(0.0f, 0.0f, -m_scaleZ);
			positive_location_z.reset(0.0f, 0.0f, m_scaleZ);
		}

		void getSpaceLocationX(Vector3 & negative,Vector3& positive) const{
			negative = negative_location_x;
			positive = positive_location_x;
		}

		void getSpaceLocationY(Vector3& negative, Vector3& positive) const {
			negative = negative_location_y;
			positive = positive_location_y;
		}

		void getSpaceLocationZ(Vector3& negative, Vector3& positive) const {
			negative = negative_location_z;
			positive = positive_location_z;
		}

	};
}