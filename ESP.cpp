
#include "ESP.h"




void Renderer::DrawBox(const Vector2& top, const Vector2& bottom, ImU32 color)
{
    float height = bottom.y - top.y;
    float width = height / 2.0f;
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    drawList->AddRect(ImVec2(top.x - width / 2, top.y),
        ImVec2(top.x + width / 2, bottom.y),
        color, 0.0f, 0, 1.5f);
}
  
void Renderer::DrawSkeleton(const std::vector<Vector2>& bones, ImU32 color)
{
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        for (size_t i = 0; i < bones.size() - 1; ++i)
        {
            drawList->AddLine(ImVec2(bones[i].x, bones[i].y),
                              ImVec2(bones[i + 1].x, bones[i + 1].y), 
                               color, 1.0f);
        }
}
   