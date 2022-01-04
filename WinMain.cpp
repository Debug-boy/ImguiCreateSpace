#include "M_IMGUI.h"
#include "Camera.hpp"
#include "Axis.hpp"
#include <winbase.h>
#include <string>

using namespace infinity;
using namespace std;

static float g_Roll = -90.0f, g_Pitch = 135.0f, g_Yaw;
static float g_scale = 10.0f;
static float g_ImguiWidth = g_mWindowRect.w;
static float g_ImguiHeight = 300.0f;
static infinity::Translation g_Translation(0.0f, 270.0f, -165.0f);
static infinity::Vector2 g_SpaceRenderSize(g_mWindowRect.w, 250.0f);
static infinity::Camera g_Camera(Rotation(g_Roll, g_Pitch, g_Yaw), g_Translation, g_SpaceRenderSize);

static float axisXmax = 100.0f;
static float axisYmax = 100.0f;
static float axisZmax = 400.0f;

typedef float (*fun_callback)(float x);

float f(float x) {
    return sqrtf(1.0f - powf(fabs(x) - 1.0, 2.0f));
}

float g(float x) {
    return acosf(1.0f - fabs(x)) - 3.0f;
}

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


void drawCube(const Vector3& worldLocation, float cubeSize, const ImColor& color) {
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


float physicsLine(float x) {
    return x * x - 16;
}

float basicLine(float x) {
    return 4 * x - 4;
}

void drawFunction(fun_callback f, const ImColor& color, const float& thickness = 1.0f, const char* label = "",float function_rate = 1.0f, const float begin = -axisXmax, const float& end = axisXmax, bool mainZ = true) {
    Vector2 pre, cur;
    float localBegin = begin / g_scale;
    float localEnd = end / g_scale;
    Vector2 labelPosition{};

    g_Camera.worldToScreen(Vector3(localBegin * g_scale, 0.0f, f(localBegin) * g_scale), labelPosition);

    for (float x = localBegin; x <= localEnd; x += 0.1f, pre = cur) {
        float y = f(x);
        Vector3 vector = mainZ ? Vector3(x * g_scale, 0.0f, y * g_scale * function_rate) : Vector3(x * g_scale, y * g_scale, 0.0f);
        if (g_Camera.worldToScreen(vector, cur) && pre.x > 0.0f) {
            ImGui::GetForegroundDrawList()->AddLine(pre, cur, color, thickness);
        }
    }
    
    if (labelPosition.x < 0)labelPosition.x = 0; else labelPosition.x -= 60.0f;
    if (labelPosition.x > g_mWindowRect.w) labelPosition.x = g_mWindowRect.w - 60.0f;

    if (labelPosition.y < 0)labelPosition.y = 0; else labelPosition.y += 15.0f;
    if (labelPosition.y > g_mWindowRect.h)labelPosition.y = g_mWindowRect.h - 60.0f;
    ImGui::GetForegroundDrawList()->AddText(labelPosition, color, label);
}

VOID RenderCallBack() {
    
    ImGui::Begin("Infinity 3DSpace");
    ImGui::SetWindowSize(ImVec2(g_ImguiWidth, g_ImguiHeight));
    ImGui::SetWindowPos(ImVec2(0, g_mWindowRect.h - g_ImguiHeight));
                
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(g_ImguiWidth, g_mWindowRect.h - g_ImguiHeight), ImColor(0x39, 0x39, 0x39));

    ImGui::Text("Current application fps:%f", ImGui::GetIO().Framerate);
    
    
    ImGui::NewLine();
    static struct {
        bool roll; bool pitch; bool yaw;
    }auto_rotate;
    ImGui::Text("CameraRotation");
    ImGui::Checkbox("Auto Rotate", &auto_rotate.roll); ImGui::SameLine(); ImGui::SliderFloat("Roll", &g_Roll, -180.0f, 180.0f);
    ImGui::Checkbox("Auto  pitch", &auto_rotate.pitch);  ImGui::SameLine(); ImGui::SliderFloat("Pitch", &g_Pitch, -360.0f, 360.0f);
    ImGui::Checkbox("Auto    yaw", &auto_rotate.yaw); ImGui::SameLine(); ImGui::SliderFloat("Yaw", &g_Yaw, -180.0f, 180.0f);
    g_Camera.setRotation(g_Roll * M_RAD, g_Pitch * M_RAD, g_Yaw * M_RAD);

    if (auto_rotate.pitch) {
        static bool reverse = false;
        if (g_Pitch <= -180.0f)reverse = true;
        else if (g_Pitch >= 180.0f)reverse = false;
        reverse ? ++g_Pitch : --g_Pitch;
    }
    
    ImGui::NewLine();
    ImGui::Text("CameraTranslation");
    ImGui::SliderFloat("TranslateX", &g_Translation.x, -500.0f, 500.0f);
    ImGui::SliderFloat("TranslateY", &g_Translation.y, -500.0f, 500.0f);
    ImGui::SliderFloat("TranslateZ", &g_Translation.z, -500.0f, 500.0f);
    g_Camera.setTranslation(g_Translation);
    
    static infinity::Vector2 axisBegin,axisEnd;
    static infinity::Vector2 quadrantVertex[4];
    //++rotateAngle;

    if (g_Camera.worldToScreen(Vector3(-axisXmax - 5.0f, 0.0f, 0), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(axisXmax + 5.0f, 0.0f, 0.0f), axisEnd)) {

            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(1.0f, 0.0f, 0.0f), 3.0f);
            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(1.0f, 0.0f, 0.0f), "-X");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(1.0f, 0.0f, 0.0f), "X");
        }
    }
    if (g_Camera.worldToScreen(infinity::Vector3(0.0f, -axisYmax - 5.0f, 0.0f), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(0.0f, axisYmax + 5.0f, 0.0f), axisEnd)) {

            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(0.0f, 1.0f, 0.0f), 3.0f);
            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(0.0f, 1.0f, 0.0f), "-Y");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(0.0f, 1.0f, 0.0f), "Y");

            
        }
    }
    if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 0.0f, -axisZmax), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 0.0f, axisZmax), axisEnd)) {
            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(0.0f, 0.0f, 1.0f), 3.0f);

            //ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(0.0f, 0.0f, 1.0f), "-Z");
            //ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(0.0f, 0.0f, 1.0f), "Z");
        }
    }

    g_Camera.worldToScreen(Vector3(100.0f, 100.0f, 0), quadrantVertex[0]);
    g_Camera.worldToScreen(Vector3(-100.0f, 100.0f, 0), quadrantVertex[1]);
    g_Camera.worldToScreen(Vector3(-100.0f, -100.0f, 0), quadrantVertex[2]);
    g_Camera.worldToScreen(Vector3(100.0f, -100.0f, 0), quadrantVertex[3]);
    ImGui::GetForegroundDrawList()->AddPolyline(quadrantVertex, 4, ImColor(0.0f, 1.0f, 1.0f), 1, 2.0f);

    for (float variable = -axisXmax,zScale = axisZmax / 100.0f; variable <= axisXmax; variable += g_scale) {

        if (std::abs(0.0f - variable) <= 1e-1f)
            continue;

        Vector2 rulur;

        if (g_Camera.worldToScreen(Vector3(variable, 0.0f, -8.0f), rulur)) {
            ImColor color = ImColor(1.0f, 0.0f, 0.0f);
            ImGui::GetForegroundDrawList()->AddText(rulur, color, to_string(int(variable / g_scale)).c_str());
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(rulur.x, rulur.y), ImVec2(rulur.x, rulur.y - 4), color);
        }
        
        if (g_Camera.worldToScreen(Vector3(0.0f, variable, -8.0f), rulur)) {
            ImColor color = ImColor(0.0f, 1.0f, 0.0f);
            ImGui::GetForegroundDrawList()->AddText(rulur, color, to_string(int(variable / g_scale)).c_str());
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(rulur.x, rulur.y), ImVec2(rulur.x, rulur.y - 4), color);
        }

        //z轴缩放有点特殊单独处理一下
        if (g_Camera.worldToScreen(Vector3(3.0f, 0.0f, variable * zScale), rulur)) {
            ImGui::GetForegroundDrawList()->AddText(rulur, ImColor(1.0f, 0.5f, 0.5f), to_string(int((zScale* variable) / g_scale)).c_str());
        }
    }

    //drawHeart();
    //drawPhysicsLine();
    drawFunction(physicsLine, ImColor(0.0f, 1.0f, 0.0f), 4.0f, "z = x^2 - 8");
    drawFunction(basicLine, ImColor(0.0f, 1.0f, 1.0f), 4.0f, "z = 4x - 4");
    drawFunction(sin, ImColor(1.0f, 1.0f, 0.0f), 4.0f, "z = 4sin(x)", 4.0f);
    drawCube(Vector3(0, 0, -axisZmax), 5, ImColor(0xFF, 0xFF, 0, 0xFF));

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