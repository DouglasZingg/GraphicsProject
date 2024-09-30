#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
using namespace DirectX;

class Camera
{
private:
	XMMATRIX camera = XMMatrixTranslation(0, 0, -20);
public:
	void UpdateViewMatrix();
	
	XMMATRIX ReturnCamera();
	
	XMVECTOR ReturnCameraR();
	
	void SetSpotLight(XMFLOAT4 spotLight);
	XMFLOAT4 GetSpotLight();
	XMFLOAT4 light;
	
	void SetFOV(float x);
	float GetFOV();
	float fov;
};