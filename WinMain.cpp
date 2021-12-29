#include "M_IMGUI.h"
#include "Camera.hpp"
#include <winbase.h>

using namespace infinity;

static float g_Roll, g_Pitch, g_Yaw;
static infinity::Translation g_Translation(0, 0, 0);
static infinity::Vector2 g_SpaceRenderSize(g_mWindowRect.w, 500);
static infinity::Camera g_Camera(Rotation(g_Roll, g_Pitch, g_Yaw), g_Translation, g_SpaceRenderSize);

float f(float x) {
    return sqrtf(1.0f - powf(fabs(x) - 1.0, 2.0f));
}

float g(float x) {
    return acosf(1.0f - fabs(x)) - 3.0f;
}

static float rotateAngle;

Vector2 relativeRotation(const Vector2& origin, const Vector2& relative, float angle) {
    float dx = origin.x - relative.x;
    float dy = origin.y - relative.y;
    return Vector2(
        (float)(dx * cosf(angle * M_RAD) - dy * sinf(angle * M_RAD) + relative.x),
        (float)(dx * sinf(angle * M_RAD) + dy * cosf(angle * M_RAD) + relative.y)
        );
}

void drawHeart() {
    Vector2 top1, top2;
    Vector2 pre1, pre2;
    for (float x = -2.0f; x <= 2.0f; x += 0.01f, pre1 = top1, pre2 = top2) {

        float y1 = f(x) * 30;
        float y2 = g(x) * 30;

        g_Camera.worldToScreen(infinity::Vector3(x * 30, y1, 0), top1);
        g_Camera.worldToScreen(infinity::Vector3(x * 30, y2, 0), top2);

        if (pre1.x > 0)
            ImGui::GetForegroundDrawList()->AddLine(pre1, top1, ImColor(244, 154, 180, 0xff), 2);

        if (pre2.x > 0)
            ImGui::GetForegroundDrawList()->AddLine(pre2, top2, ImColor(244, 154, 180, 0xff), 2);
    }
}

void drawPhysicsLine() {
    Vector2 cur, pre;
    for (float x = -4; x <= 4; x += 0.01f, pre = cur) {
        float y = x * x;

        g_Camera.worldToScreen(infinity::Vector3(x * 30, -y * 30, 0), cur);
        if(pre.x>0)
            ImGui::GetForegroundDrawList()->AddLine(pre, cur, ImColor(0, 255, 0, 0xff), 2);

    }
}

void drawCube(const Vector3& worldLocation, float cubeSize, const ImColor &color) {
    Vector2 topCur, topPre;
    Vector2 bottomCur, bottomPre;
    for (auto i = 1; i <= 5; i++, topPre = topCur, bottomPre = bottomCur) {
        float x = worldLocation.x + cosf((45 + i * 90) * M_RAD) * cubeSize;
        float y = worldLocation.y + sinf((45 + i * 90) * M_RAD) * cubeSize;
        g_Camera.worldToScreen(Vector3(x, y, (worldLocation.z - cubeSize * 0.5f)), topCur);
        g_Camera.worldToScreen(Vector3(x, y, (worldLocation.z + cubeSize * 0.5f)), bottomCur);
        if (i > 1) {
            ImGui::GetForegroundDrawList()->AddLine(topPre, topCur, color, 2);
            ImGui::GetForegroundDrawList()->AddLine(bottomPre, bottomCur, color, 2);
            ImGui::GetForegroundDrawList()->AddLine(topPre, bottomPre, color, 2);
            ImGui::GetForegroundDrawList()->AddLine(topCur, bottomCur, color, 2);
        }
    }

}

VOID RenderCallBack() {
    
    ImGui::Begin("Infinity 3DSpace");
    ImGui::SetWindowSize(ImVec2(g_mWindowRect.w, 300));
    ImGui::SetWindowPos(ImVec2(0, g_mWindowRect.h - 300));

    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0, 0), g_SpaceRenderSize, ImColor(0x69, 0x69, 0x69));

    ImGui::Text("Current application fps:%f", ImGui::GetIO().Framerate);
    
    
    ImGui::NewLine();
    ImGui::Text("CameraRotation");
    ImGui::SliderFloat("Roll", &g_Roll, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &g_Pitch, -180.0f, 180.0f);
    ImGui::SliderFloat("Yaw", &g_Yaw, -180.0f, 180.0f);
    g_Camera.setRotation(g_Roll * M_RAD, g_Pitch * M_RAD, g_Yaw * M_RAD);

    
    ImGui::NewLine();
    ImGui::Text("CameraTranslation");
    ImGui::SliderFloat("TranslateX", &g_Translation.x, -500.0f, 500.0f);
    ImGui::SliderFloat("TranslateY", &g_Translation.y, -500.0f, 500.0f);
    ImGui::SliderFloat("TranslateZ", &g_Translation.z, -500.0f, 500.0f);
    g_Camera.setTranslation(g_Translation);
    
    static infinity::Vector2 axisBegin,axisEnd;
    static infinity::Vector2 quadrantVertex[4];

    //++rotateAngle;

    g_Camera.worldToScreen(Vector3(99.0f, 99.0f, 0), quadrantVertex[0]);
    g_Camera.worldToScreen(Vector3(-99.0f, 99.0f, 0), quadrantVertex[1]);
    g_Camera.worldToScreen(Vector3(-99.0f, -99.0f, 0), quadrantVertex[2]);
    g_Camera.worldToScreen(Vector3(99.0f, -99.0f, 0), quadrantVertex[3]);
    ImGui::GetForegroundDrawList()->AddPolyline(quadrantVertex, 4, ImColor(0.0f, 1.0f, 1.0f), 1, 2.0f);

    if (g_Camera.worldToScreen(infinity::Vector3(-99.0f, 0.0f, 0.0f), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(99.0f, 0.0f, 0.0f), axisEnd)) {
            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(1.0f, 0.0f, 0.0f), 3.0f);
            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(1.0f, 0.0f, 0.0f), "-X");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(1.0f, 0.0f, 0.0f), "X");
        }
    }
    if (g_Camera.worldToScreen(infinity::Vector3(0.0f, -99.0f, 0.0f), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 99.0f, 0.0f), axisEnd)) {

            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(0.0f, 1.0f, 0.0f), 3.0f);
            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(0.0f, 1.0f, 0.0f), "-Y");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(0.0f, 1.0f, 0.0f), "Y");

        }
    }
    if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 0.0f, -99.0f), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 0.0f, 99.0f), axisEnd)) {
            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(0.0f, 0.0f, 1.0f), 3.0f);

            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(0.0f, 0.0f, 1.0f), "-Z");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(0.0f, 0.0f, 1.0f), "Z");
        }
    }

    drawHeart();
    //drawPhysicsLine();
    //drawCube(Vector3(0, 0, -99), 5, ImColor(0xFF, 0xFF, 0, 0xFF));

    ImGui::End();
    SleepEx(ImGui::GetIO().Framerate * 2, true);
}



// Main code
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow) {
    
    if (!InitCreateDrawWindow(NULL))
        return -1;
	
    SetRenderCallback(RenderCallBack);

    return 0;
}