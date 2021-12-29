#pragma once
#include "imgui.h"

namespace infinity {
	class Vector2 :public ImVec2 {
	public:
		Vector2() :ImVec2() {}
		Vector2(float x, float y) :ImVec2(x, y) {};
	
	};



}
