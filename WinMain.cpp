#include "M_IMGUI.h"
#include "Camera.hpp"
#include "Axis.hpp"
#include <winbase.h>
#include <string>

using namespace infinity;
using namespace std;

static float g_Roll = 145.0, g_Pitch = 135.0f, g_Yaw;
static float g_scale = 10.0f;
static float g_ImguiWidth = 300;
static float g_ImguiHeight = 300.0f;
static infinity::Translation g_Translation(0.0f, 0.0f, -270.0f);
static infinity::Vector2 g_SpaceRenderSize(g_mWindowRect.w + 300, g_mWindowRect.h);
static infinity::Camera g_Camera(Rotation(g_Roll, g_Pitch, g_Yaw), g_Translation, g_SpaceRenderSize);

static float axisXmax = 100.0f;
static float axisYmax = 100.0f;
static float axisZmax = 100.0f;

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
        float y = worldLocation.y + sinf((45 + i * 90) * M_RAD) * cubeSize * g_scale;
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
    return x * x - 4;
}

float basicLine(float x) {
    return 4 * x - 4;
}

void drawFunction(fun_callback f, const ImColor& color, const float& thickness = 1.0f, const char* label = "",float function_rate = 1.0f, const float begin = -axisXmax, const float& end = axisXmax, bool mainZ = true) {
    Vector2 pre, cur;
    float localBegin = begin;
    float localEnd = end;
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

void putpixle(const ImVec2 &vec2,ImColor color) {
    ImGui::GetForegroundDrawList()->AddLine(vec2, ImVec2(vec2.x + 1, vec2.y + 1), color);
}

void drawBall(const Vector3& vector,Rotation& rotation,bool isR, ImColor color, double r = 1.0, bool isHash = false) {
    double u = 2.0 * 3.141592653589793238462643383279;
    double v = 3.141592653589793238462643383279;
    Vector3 temp;
    Vector2 pre, cur;
    for (double x = 0; x <= u; x += 0.1) {
        for (double y = 0; y < v; y += 0.1, pre = cur) {
            double _x = vector.x * g_scale + cos(x) * sin(y) * g_scale * r;
            double _y = vector.y * g_scale + sin(y) * sin(x) * g_scale * r;
            double _z = vector.z * g_scale + cos(y) * g_scale * r;
            if (isR)
                rotation.rotateToWorld(Vector3(_x, _y, _z), temp);
            else
                temp.reset(_x, _y, _z);
            g_Camera.worldToScreen(temp, cur);
            if (isHash) {
                putpixle(cur, color);
            }
            else {
                if (y > 0) {
                    ImGui::GetForegroundDrawList()->AddLine(pre, cur, color);
                }
            }
        }
    }
}

void drawBall2(const Vector3& vector, ImColor color, double r = 1.0, bool isHash = false) {
    double u = 2.0 * 3.141592653589793238462643383279;
    double v = 3.141592653589793238462643383279;
    Vector3 temp;
    Vector2 pre, cur;
    for (double x = 0; x <= u; x += 0.1) {
        for (double y = 0; y < v; y += 0.1, pre = cur) {
            double _x = vector.x * g_scale + cos(x) * sin(y) * g_scale * r;
            double _y = vector.y * g_scale + sin(y) * sin(x) * g_scale * r;
            double _z = vector.z * g_scale + cos(y) * g_scale * r;
            g_Camera.worldToScreen(Vector3(_x, _y, _z), cur);
            if (isHash) {
                putpixle(cur, color);
            }else {
                if (y > 0) {
                    ImGui::GetForegroundDrawList()->AddLine(pre, cur, color);
                }
            }
        }
    }
}

void drawCircle(const Vector3& origin, double r,const Rotation &rotation, ImColor color = 0,float thinkness = 1.0f) {
    static const unsigned int sideNumber = 180;
    static const double rotateAngle = 360.0 / sideNumber;
    Vector3 position(0, 0, origin.z * g_scale);
    Vector3 realPosition{};
    Vector2 pre, cur;
    Rotation localRotate = rotation;
    for (int i = 0; i <= sideNumber; ++i, pre = cur) {
        double real_x = rotateAngle * i * M_RAD;
        position.x = origin.x * g_scale + cos(real_x) * r * g_scale;
        position.y = origin.y * g_scale + sin(real_x) * r * g_scale;
        localRotate.rotateToWorld(position, realPosition);
        g_Camera.worldToScreen(realPosition, cur);
        if (i) ImGui::GetForegroundDrawList()->AddLine(pre, cur, color, thinkness);
    }
}

class Planet {

public:
    std::string mName = "Unknow planet";
    ImColor mColor;
    double mSize;
    double mSpeed = 0.5f;
    double mAngle = 0.0f;
    Rotation mRotate;//绕圈旋转
    Rotation trackRotate;
    Vector3 worldPosition;

    Planet(string name,const Vector3 &init_position,double size = 0.5f,double speed  = 0.5f,ImColor color = 0) {
        mName = name;
        worldPosition = init_position;
        mSpeed = speed;
        mColor = color;
        mSize = size;
    }
 
    void render(bool isHash = true,double trackSize = 0.0) {
        //Vector3 temp, temp2;
        //mAngle += mSpeed * M_RAD;
        //mRotate.setYaw(-mAngle);
        //mRotate.setRoll(trackOffsetAngle * M_RAD);

        //if (trackOffsetAngle > 0) {
        //    mRotate.setYaw(0);
        //}

        //Vector2 namePos;
        //Vector3 tempPosition(cos(mAngle) * trackSize, sin(mAngle) * trackSize, 0);
        //Vector3 nameWorldPos(cos(mAngle * 2) * trackSize * g_scale,sin(mAngle * 2) * trackSize * g_scale, worldPosition.z);

        //mRotate.rotateToWorld(tempPosition, temp);
        //drawBall(temp, mRotate, true, mColor, mSize, isHash);

        //if (trackOffsetAngle > 0) {
        //    mRotate.setYaw(0);
        //    mRotate.setRoll(-trackOffsetAngle * M_RAD);
        //    mRotate.setPitch(-trackOffsetAngle * M_RAD);
        //    nameWorldPos.reset(cos(mAngle + trackOffsetAngle *M_RAD) * trackSize * g_scale, 
        //        sin(mAngle + trackOffsetAngle * M_RAD) * trackSize * g_scale, worldPosition.z);
        //}

        //mRotate.rotateToWorld(nameWorldPos, temp2);
        //g_Camera.worldToScreen(temp2, namePos);

        //ImGui::GetForegroundDrawList()->AddText(namePos, ImColor(0xff, 0xff, 0xff, 0xff), mName.c_str());
        drawBall(worldPosition, mRotate, true, mColor, mSize, isHash);
        drawCircle(worldPosition, trackSize, trackRotate, mColor, 2.0f);
    }

};



VOID RenderCallBack() {
    
    ImGui::Begin("Infinity 3DSpace");
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(g_ImguiWidth + 50, g_mWindowRect.h));
                
    //ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(g_ImguiWidth, g_mWindowRect.h - g_ImguiHeight), ImColor(0x39, 0x39, 0x39));

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
    ImGui::SliderFloat("TranslateX", &g_Translation.x, -1500, 1500);
    ImGui::SliderFloat("TranslateY", &g_Translation.y, -1500, 1500);
    ImGui::SliderFloat("TranslateZ", &g_Translation.z, -1500, 1500);
    g_Camera.setTranslation(g_Translation);
    
    static infinity::Vector2 axisBegin,axisEnd;
    static infinity::Vector2 quadrantVertex[4];
    //++rotateAngle;

    if (g_Camera.worldToScreen(Vector3(-axisXmax - 5.0f, 0.0f, 0), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(axisXmax + 5.0f, 0.0f, 0.0f), axisEnd)) {

            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(1.0f, 0.0f, 0.0f, 0.1f), 3.0f);
            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(1.0f, 0.0f, 0.0f), "-X");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(1.0f, 0.0f, 0.0f), "X");
        }
    }
    if (g_Camera.worldToScreen(infinity::Vector3(0.0f, -axisYmax - 5.0f, 0.0f), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(0.0f, axisYmax + 5.0f, 0.0f), axisEnd)) {

            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(0.0f, 1.0f, 0.0f, 0.1f), 3.0f);
            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(0.0f, 1.0f, 0.0f), "-Y");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(0.0f, 1.0f, 0.0f), "Y");

            
        }
    }
    if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 0.0f, -axisZmax), axisBegin)) {
        if (g_Camera.worldToScreen(infinity::Vector3(0.0f, 0.0f, axisZmax), axisEnd)) {
            ImGui::GetForegroundDrawList()->AddLine(axisBegin, axisEnd, ImColor(0.0f, 0.0f, 1.0f, 0.1f), 3.0f);

            ImGui::GetForegroundDrawList()->AddText(axisBegin, ImColor(0.0f, 0.0f, 1.0f), "-Z");
            ImGui::GetForegroundDrawList()->AddText(axisEnd, ImColor(0.0f, 0.0f, 1.0f), "Z");
        }
    }

    g_Camera.worldToScreen(Vector3(100.0f, 100.0f, 0), quadrantVertex[0]);
    g_Camera.worldToScreen(Vector3(-100.0f, 100.0f, 0), quadrantVertex[1]);
    g_Camera.worldToScreen(Vector3(-100.0f, -100.0f, 0), quadrantVertex[2]);
    g_Camera.worldToScreen(Vector3(100.0f, -100.0f, 0), quadrantVertex[3]);
    ImGui::GetForegroundDrawList()->AddPolyline(quadrantVertex, 4, ImColor(0.0f, 1.0f, 1.0f, 0.1f), 1, 2.0f);

    for (float variable = -axisXmax; variable <= axisXmax; variable += g_scale) {

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
        if (g_Camera.worldToScreen(Vector3(3.0f, 0.0f, variable), rulur)) {
            ImGui::GetForegroundDrawList()->AddText(rulur, ImColor(1.0f, 0.5f, 0.5f), to_string(int(variable / g_scale)).c_str());
        }

    }

    //drawHeart();
    //drawPhysicsLine();
    static float xxx,yyy,zzz;
    static float xxx1, yyy1, zzz1;
    ImGui::SliderFloat("tarck_xxx", &xxx, -180.0f, 180.0f);
    ImGui::SliderFloat("tarck_yyy", &yyy, -180.0f, 180.0f);
    ImGui::SliderFloat("tarck_zzz", &zzz, -180.0f, 180.0f);
    ImGui::SliderFloat("m_xxx1", &xxx1, -180.0f, 180.0f);
    ImGui::SliderFloat("m_yyy1", &yyy1, -180.0f, 180.0f);
    ImGui::SliderFloat("m_zzz1", &zzz1, -180.0f, 180.0f);
   // drawCircle(Vector3(0, 0, 0), 2, Rotation(xxx * M_RAD, yyy * M_RAD, zzz * M_RAD), ImColor(0xff, 0, 0, 0xff));

    //drawBall(Vector3(0, 0, 0), ImColor(0xff, 0x0, 0x0, 0xff), 3);//sun
    static Planet sun("sun", Vector3(0, 0, 0), 10.0, 0.197f, ImColor(0xff, 0x0, 0x0, 0xff));
    static Planet mercury("mercury", Vector3(13, 0, 0), 0.5f, 0.4789, ImColor(0xff, 0xff, 0, 0xff));
    static Planet venus("venus", Vector3(16, 0, 0), 0.82f, 0.3503, ImColor(0xe6, 0x91, 0xa, 0xff));
    static Planet eath("eath", Vector3(26, 0, 0), 1.0, 0.29783, ImColor(0x4c, 0x9d, 0xef, 0xff));
    static Planet mars("mars", Vector3(32, 0, 0), 0.15, 0.2413, ImColor(245, 145, 67, 0xff));
    static Planet jupiter("jupiter", Vector3(46, 0, 0), 5.5, 0.1306, ImColor(214, 147, 121, 0xff));
    static Planet saturn("saturn", Vector3(64, 0, 0), 3.7, 0.0964, ImColor(0xb4, 0x5f, 0x6, 0xff));
    static Planet uranus("uranus", Vector3(72, 0, 0), 3, 0.0681, ImColor(0x0, 0xff, 0xff, 0xff));
    static Planet neptune("neptune", Vector3(100, 0, 0), 2, 0.0543, ImColor(0x0, 0x2f, 0xff, 0xff));

    //drawBall2(Vector3(0, 0, 0), ImColor(0xff, 0x0, 0x0, 0xff), 6.0);

    static Planet sun2("sun", Vector3(5, 5, 0), 4.0, 0.197f, ImColor(0xff, 0x0, 0x0, 0xff));
    sun2.trackRotate.set(xxx* M_RAD, yyy* M_RAD, zzz* M_RAD);
    sun2.mRotate.set(xxx1* M_RAD, yyy1* M_RAD, zzz1* M_RAD);
    sun2.render(false, 4);


    //sun.render(false, 0);
    //sun.mRotate.set(xxx * M_RAD, yyy * M_RAD, zzz * M_RAD);
    //mercury.render(true, 13, xxx);
    //venus.render(true, 16, 3.4 * 10 * M_RAD);
    //eath.render(true, 26, 0);
    //mars.render(true, 32, 0.093 * 10 * M_RAD);
    //jupiter.render(true, 46);
    //saturn.render(true, 54);
    //uranus.render(true, 62);
    //neptune.render(true, 80);
    //drawCircle(Vector3(0, 0, 0), 5, Rotation(0, 0, 0), ImColor(208, 160, 0x0, 0xff), 2.0f);

   

    //drawCircle(Vector3(0, 0, 0), 15, Rotation(0, 0, 0), ImColor(0x0, 0xff, 0xff, 0xff), 2.0f);
    //drawCircle(Vector3(0, 0, 0), 20, Rotation(0, 0, 0), ImColor(0xff, 0xff, 0xff, 0xff), 2.0f);
    //drawCircle(Vector3(0, 0, 0), 30, Rotation(0, 0, 0), ImColor(0xff, 0xff, 0xff, 0xff), 2.0f);

    //drawBall(Vector3(-10, 10, 0), ImColor(0xff, 0x0, 0x0, 0xff), 0.5f, true);
    //drawBall(Vector3(10, -10, 0), ImColor(0xff, 0xff, 0xff, 0xff), 0.5f, true);



    //drawFunction(physicsLine, ImColor(0.0f, 1.0f, 0.0f), 4.0f, "z = x^2 - 8");
    //drawFunction(basicLine, ImColor(0.0f, 1.0f, 1.0f), 4.0f, "z = 4x - 4");
    //drawFunction(sin, ImColor(1.0f, 1.0f, 0.0f), 4.0f, "z = 4sin(x)", 4.0f);
    //drawFunction(s, ImColor(1.0f, 1.0f, 0.0f), 4.0f, "z = sn(x)", 4.0f);
    //drawCube(Vector3(0, 0, -axisZmax), 4, ImColor(0xFF, 0xFF, 0, 0xFF));
    //drawCube(Vector3(0, 0, 0), 1, ImColor(0xFF, 0xFF, 0, 0xFF));

    ImGui::End();
    //SleepEx(ImGui::GetIO().Framerate, true);
}



// Main code
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow) {
    
    if (!InitCreateDrawWindow(NULL))
        return -1;
	
    SetRenderCallback(RenderCallBack);

    return 0;
}