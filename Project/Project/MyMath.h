#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
using namespace DirectX;

struct HUD
{
    float xyzw[4];
    float rgba[4];
}MyHUD;

struct VEC_2D
{
    float x, y;
};

struct VEC_3D
{
    float x, y, z;
};

struct Vertex
{
    VEC_3D pos;
    VEC_2D texCoord;
    VEC_3D normal;
};

struct WVP
{
	XMFLOAT4X4 wMatrix;
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
}MyMatricies;