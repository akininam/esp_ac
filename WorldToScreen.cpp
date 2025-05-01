
#include "WorldToScreen.h"
#include <imgui/imgui.h>

bool WorldToScreen(const Vector3& pos, Vector2& screen, const ViewMatrix& vm, int screenWidth, int screenHeight)
{
	float clipW = pos.x * vm.matrix[3] + pos.y *vm.matrix[7] + pos.z * vm.matrix[11] + vm.matrix[15];
	if (clipW < 0.01f) return false;

	float clipX = pos.x * vm.matrix[0] + pos.y * vm.matrix[4] + pos.z * vm.matrix[8] + vm.matrix[12];
	float clipY = pos.x * vm.matrix[1] + pos.y * vm.matrix[5] + pos.z * vm.matrix[9] + vm.matrix[13];
	
	float x = clipX / clipW;
	float y = clipY / clipW;

	const ImVec2 size = ImGui::GetIO().DisplaySize;

	screen.x = (size.x * 0.5f * x)  + (x + size.x * 0.5f);
	screen.y = (size.y * 0.5f * y) + (y + size.y * 0.5f);
	return true;
}
