#pragma once

#include "WorldToScreen.h"
#include <imgui/imgui.h>
#include <vector>

namespace Renderer
{
	void DrawBox(const Vector2& top, const Vector2& bottom, ImU32 color);
	void DrawSkeleton(const std::vector<Vector2>& bones, ImU32 color);
}

