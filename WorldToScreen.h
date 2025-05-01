#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

struct Vector3 
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};

struct ViewMatrix
{
	float matrix[16];
};

bool WorldToScreen(const Vector3& pos, Vector2& screen, const ViewMatrix& vm, int screenWidth, int screenHeight);