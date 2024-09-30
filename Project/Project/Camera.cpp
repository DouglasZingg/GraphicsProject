#include "Camera.h"

void Camera::UpdateViewMatrix()
{
	if (GetAsyncKeyState('W'))//Foward
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, 0.5f), camera);
	}
	if (GetAsyncKeyState('S'))//Back
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, -0.5f), camera);
	}
	if (GetAsyncKeyState('A'))//Left
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(-0.5f, 0, 0), camera);
	}
	if (GetAsyncKeyState('D'))//Right
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0.5f, 0, 0), camera);
	}
	if (GetAsyncKeyState(VK_UP))//Look Up
	{
		camera = XMMatrixMultiply(XMMatrixRotationX(-0.01f), camera);

		XMMATRIX sRotate = XMMatrixRotationX(-0.01f);
		XMVECTOR sLightDir = XMLoadFloat4(&light);
		sLightDir = XMVector3Transform(sLightDir, sRotate);
		XMStoreFloat4(&light, sLightDir);
		SetSpotLight(light);
	}
	if (GetAsyncKeyState(VK_DOWN))//Look Down
	{
		camera = XMMatrixMultiply(XMMatrixRotationX(0.01f), camera);

		XMMATRIX sRotate = XMMatrixRotationX(0.01f);
		XMVECTOR sLightDir = XMLoadFloat4(&light);
		sLightDir = XMVector3Transform(sLightDir, sRotate);
		XMStoreFloat4(&light, sLightDir);
		SetSpotLight(light);
	}
	if (GetAsyncKeyState(VK_LEFT))//Look Left
	{
		XMVECTOR pos = camera.r[3];
		camera.r[3] = XMVectorSet(0, 0, 0, 1);
		camera = XMMatrixMultiply(camera, XMMatrixRotationY(-0.01f));
		camera.r[3] = pos;

		XMMATRIX sRotate = XMMatrixRotationY(-0.01f);
		XMVECTOR sLightDir = XMLoadFloat4(&light);
		sLightDir = XMVector3Transform(sLightDir, sRotate);
		XMStoreFloat4(&light, sLightDir);
		SetSpotLight(light);
	}
	if (GetAsyncKeyState(VK_RIGHT))//Look Right
	{
		XMVECTOR pos = camera.r[3];
		camera.r[3] = XMVectorSet(0, 0, 0, 1);
		camera = XMMatrixMultiply(camera, XMMatrixRotationY(0.01f));
		camera.r[3] = pos;

		XMMATRIX sRotate = XMMatrixRotationY(0.01f);
		XMVECTOR sLightDir = XMLoadFloat4(&light);
		sLightDir = XMVector3Transform(sLightDir, sRotate);
		XMStoreFloat4(&light, sLightDir);
		SetSpotLight(light);
	}
	if (GetAsyncKeyState('R'))//Fly up
	{
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, 0.5f, 0));
	}
	if (GetAsyncKeyState('F'))//Fly Down
	{
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, -0.5f, 0));
	}
	if (GetAsyncKeyState('Z'))//Fly Down
	{
		fov = fov + 0.1f;
		if (fov > 7.0f)
			fov = 7.0f;
	}
	if (GetAsyncKeyState('X'))//Fly Down
	{
		fov = fov - 0.1f;
		if (fov < 2.0f)
			fov = 2.0f;
	}
}

XMMATRIX Camera::ReturnCamera()
{
	return camera;
}

XMVECTOR Camera::ReturnCameraR()
{
	return camera.r[3];
}

void Camera::SetSpotLight(XMFLOAT4 spotLight)
{
	light = spotLight;
}

XMFLOAT4 Camera::GetSpotLight()
{
	return light;
}

void Camera::SetFOV(float x)
{
	fov = x;
}

float Camera::GetFOV()
{
	return fov;
}
