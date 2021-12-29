#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>

#define M_PI 3.1415926535897932384626433832795f
#define M_PI2 6.283185307179586476925286766559f
#define M_RAD 0.017453292519943295769236907684f
#define M_ANG 57.29577951308232087679815481410f;

template <typename T>
class M_VECTOR2
{

public :
	T x;
	T y;

public:
	M_VECTOR2(float _x = 0, float _y = 0) {
		this->x = _x;
		this->y = _y;
	}

};

template <typename T>
class M_VECTOR3
{
public:
	T x;
	T y;
	T z;
public:
	
	M_VECTOR3<T> &operator+(const M_VECTOR3<T>& vector3) {
		M_VECTOR3<T>vec3;
		vec3.x = this->x + vector3.x;
		vec3.y = this->y + vector3.y;
		vec3.z = this->z + vector3.z;
		return vec3;
	}

	M_VECTOR3<T>& operator-(const M_VECTOR3<T>& vector3) {
		M_VECTOR3<T>vec3;
		vec3.x = this->x - vector3.x;
		vec3.y = this->y - vector3.y;
		vec3.z = this->z - vector3.z;
		return vec3;
	}
};


typedef struct {
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;
}MATRIX;

typedef MATRIX* LP_MATRIX;

template <typename T>
class M_RECT
{
public:
	T x;
	T y;
	T w;
	T h;
};

CHAR* ToTextA(LPCSTR _Format, ...);
WCHAR* ToTextW(LPCWSTR _Format, ...);

#ifdef UNICODE
#define ToText ToTextW
#else
#define ToText ToTextA
#endif

double AngToRad(double ang);
float AngToRad(float _x);
BOOL GetWindowClientAreaData(HWND hWnd, LONG& cx, LONG& cy, LONG& cw, LONG& ch);
DWORD GetProcessID(const char* processName);
DWORD GetModuleBase(DWORD, const char* moduleName);
void PrintVec3(M_VECTOR3<FLOAT>vec3);