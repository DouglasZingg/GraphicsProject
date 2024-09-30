//headers
#include "framework.h"
#include "Project.h"
#include "XTime.h"
#include "MyMath.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Camera.h"
#include <vector>
//pre-made mesh
#include "Assets\StoneHenge.h"
#include "Assets\asteroid.h"
#include "Assets\StoneHenge_Texture.h"
#include "Assets\SkyBoxCube.h"
#include "Assets\globe-sphere.h"
#include "Assets\SS1.h"
#include "DDSTextureLoader.h"
//Shaders
#include "VertexShader.csh"
#include "PixelShader.csh"
#include "VertexMeshShader.csh"
#include "TexturePixelShader.csh"
#include "SkyBoxPS.csh"
#include "SkyBoxVS.csh"
#include "EmissivePixelShader.csh"
#include <string>

#pragma comment(lib, "d3d11.lib")

using namespace DirectX;
using namespace std;

ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;
ID3D11Buffer* vBuff;
ID3D11InputLayout* vLayout;
ID3D11VertexShader* vShader;
ID3D11PixelShader* pShader;
ID3D11Buffer* cBuff;
ID3D11Buffer* vBuffMesh;
ID3D11Buffer* skyboxvBuffMesh;
ID3D11Buffer* asteroidvBuffMesh;
ID3D11Buffer* spherevBuffMesh;
ID3D11Buffer* directionallight;
ID3D11Buffer* pointlight;
ID3D11Buffer* spotlight;
ID3D11Buffer* iBuffMesh;
ID3D11Buffer* skyboxiBuffMesh;
ID3D11Buffer* asteroidiBuffMesh;
ID3D11Buffer* sphereiBuffMesh;
ID3D11VertexShader* vMeshShader;
ID3D11InputLayout* vMeshLayout;
ID3D11PixelShader* pTextShader;
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;
ID3D11ShaderResourceView* texture;
ID3D11ShaderResourceView* skyboxtexture;
ID3D11ShaderResourceView* asteroidtexture;
ID3D11ShaderResourceView* spheretexture;
ID3D11SamplerState* sampState;
ID3D11VertexShader* skyBoxVS;
ID3D11PixelShader* skyBoxPS;
ID3D11PixelShader* EmissivePS;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
float aspectRatio = 1.0f;
unsigned int numVerts = 0;
HRESULT hr;
vector<Vertex> Sphere;
D3D11_BUFFER_DESC bDesc;
D3D11_SUBRESOURCE_DATA subData;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CleanUp();
bool LoadOBJ(const char* path, vector<Vertex>& OutVector);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT));
	MSG msg;

	//camera
	Camera camera;

	//light data
	//XMFLOAT4 directionallightdata[2];
	//directionallightdata[0] = XMFLOAT4(0, -1, 1, 0);						// directional light direction
	//directionallightdata[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			// directional light color
	//XMFLOAT4 pointlightdata[5];
	//pointlightdata[0] = XMFLOAT4(0, 0, 0, 0);								// point light pos
	//pointlightdata[1] = XMFLOAT4(10, 0, 0, 0);								// range
	//pointlightdata[2] = XMFLOAT4(0, 0.1f, 0, 0);							// att
	//pointlightdata[3] = XMFLOAT4(0.3f, 0.3f, 0.3f, 1);						// ambient
	//pointlightdata[4] = XMFLOAT4(1, 1, 0, 1);								// diffuse
	//XMFLOAT4 spotlightdata[7];
	//spotlightdata[0] = XMFLOAT4(0, 1, 0, 0);								// pos
	//spotlightdata[1] = XMFLOAT4(1000, 0, 0, 0);								// range
	//spotlightdata[2] = XMFLOAT4(0, 0, 1, 0);								// dir
	//spotlightdata[3] = XMFLOAT4(20, 0, 0, 0);								// cone
	//spotlightdata[4] = XMFLOAT4(0.4f, 0.02f, 0, 0);							// att
	//spotlightdata[5] = XMFLOAT4(0.2f, 0.2f, 0.2f, 1);						// ambient
	//spotlightdata[6] = XMFLOAT4(1, 1, 0, 1);								// diffuse

	//camera.SetSpotLight(spotlightdata[2]);
	camera.SetFOV(2.0f);

	//main loop
	while (true)
	{
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		//zbuffer
		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

		//set skybox pipeline and draw it
		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		UINT skybox_mesh_strides[] = { sizeof(_OBJ_VERT_) };
		UINT skybox_mesh_offsets[] = { 0 };
		ID3D11Buffer* skyboxmeshVB[] = { skyboxvBuffMesh };
		myCon->IASetVertexBuffers(0, 1, skyboxmeshVB, skybox_mesh_strides, skybox_mesh_offsets);
		myCon->IASetIndexBuffer(skyboxiBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->VSSetShader(skyBoxVS, 0, 0);
		myCon->PSSetShader(skyBoxPS, nullptr, 0);
		myCon->PSSetShaderResources(0, 1, &skyboxtexture);
		myCon->PSSetSamplers(0, 1, &sampState);
		myCon->IASetInputLayout(vMeshLayout);
		XMStoreFloat4x4(&MyMatricies.wMatrix, XMMatrixTranslation(XMVectorGetX(camera.ReturnCameraR()), XMVectorGetY(camera.ReturnCameraR()), XMVectorGetZ(camera.ReturnCameraR())));
		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCon->DrawIndexed(1692, 0, 0);
		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

		//output merger and rasterizer
		ID3D11RenderTargetView* tempRTV[] = { myRtv };
		myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
		myCon->RSSetViewports(1, &myPort);

		//HUD/QUAD
		//UINT strides[] = { sizeof(HUD) };
		//UINT offsets[] = { 0 };
		//ID3D11Buffer* tempVB[] = { vBuff };
		//myCon->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
		//myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//myCon->VSSetShader(vShader, 0, 0);
		//myCon->PSSetShader(pShader, 0, 0);
		//myCon->PSSetShaderResources(0, 1, &asteroidtexture);
		//myCon->PSSetSamplers(0, 1, &sampState);
		//myCon->IASetInputLayout(vLayout);
		//myCon->Draw(6, 0);

		camera.UpdateViewMatrix();
		XMStoreFloat4x4(&MyMatricies.vMatrix, XMMatrixInverse(nullptr, camera.ReturnCamera()));
		XMMATRIX temp = XMMatrixIdentity();
		temp = XMMatrixPerspectiveFovLH(3.14f / camera.GetFOV(), aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&MyMatricies.pMatrix, temp);

		//upload those matrices to the video card
		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies; // can be replaced by: memcpy(gpuBuffer.pData, &MyMatricies.wMatrix, sizeof(WVP));
		myCon->Unmap(cBuff, 0);

		//Pyrimid world matrix
		//temp = XMMatrixIdentity();
		//static float rot = 0; rot += 0.05f;
		//temp = XMMatrixTranslation(0, 3, 0);
		//XMMATRIX temp2 = XMMatrixRotationY(rot);
		//temp = XMMatrixMultiply(temp2, temp);
		//XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

		//connect constant buffer to pipline
		ID3D11Buffer* constants[] = { cBuff };
		myCon->VSSetConstantBuffers(0, 1, constants);

		//XMMATRIX dRotate = XMMatrixRotationY(0.01f);
		//XMVECTOR dLightDir = XMLoadFloat4(&directionallightdata[0]);
		//dLightDir = XMVector3Transform(dLightDir, dRotate);
		//XMStoreFloat4(&directionallightdata[0], dLightDir);

		//ID3D11Buffer* directionallightconstant[] = { directionallight };
		//hr = myCon->Map(directionallight, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		//memcpy_s(gpuBuffer.pData, sizeof(XMFLOAT4) * 2, directionallightdata, sizeof(XMFLOAT4) * 2);
		//myCon->Unmap(directionallight, 0);

		//XMMATRIX pRotate = XMMatrixRotationY(0.1f);
		//XMVECTOR pLightDir = XMLoadFloat4(&pointlightdata[0]);
		//pLightDir = XMVector3Transform(pLightDir, pRotate);
		//XMStoreFloat4(&pointlightdata[0], pLightDir);

		//ID3D11Buffer* pointlightconstant[] = { pointlight };
		//hr = myCon->Map(pointlight, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		//memcpy_s(gpuBuffer.pData, sizeof(XMFLOAT4) * 5, pointlightdata, sizeof(XMFLOAT4) * 5);
		//myCon->Unmap(pointlight, 0);

		//spotlightdata[0].x = XMVectorGetX(camera.ReturnCameraR());
		//spotlightdata[0].y = XMVectorGetY(camera.ReturnCameraR());
		//spotlightdata[0].z = XMVectorGetZ(camera.ReturnCameraR());
		//spotlightdata[2].x = camera.light.x;
		//spotlightdata[2].y = camera.light.y;
		//spotlightdata[2].z = camera.light.z;

		//ID3D11Buffer* spotlightconstant[] = { spotlight };
		//hr = myCon->Map(spotlight, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		//memcpy_s(gpuBuffer.pData, sizeof(XMFLOAT4) * 7, spotlightdata, sizeof(XMFLOAT4) * 7);
		//myCon->Unmap(spotlight, 0);

		//draw pyramid verts
		//myCon->Draw(numVerts, 0);

		//set stonehenge pipeline, map and unmap, draw stonhendge
		//UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
		//UINT mesh_offsets[] = { 0 };
		//ID3D11Buffer* meshVB[] = { vBuffMesh };
		//myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
		//myCon->IASetIndexBuffer(iBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		//myCon->VSSetShader(vMeshShader, 0, 0);
		//myCon->PSSetShader(pTextShader, nullptr, 0);
		//myCon->PSSetShaderResources(0, 1, &texture);
		////myCon->PSSetConstantBuffers(0, 1, directionallightconstant);
		////myCon->PSSetConstantBuffers(1, 1, pointlightconstant);
		////myCon->PSSetConstantBuffers(2, 1, spotlightconstant);
		//myCon->PSSetSamplers(0, 1, &sampState);
		//myCon->IASetInputLayout(vMeshLayout);
		//temp = XMMatrixIdentity();
		//XMStoreFloat4x4(&MyMatricies.wMatrix, temp);
		//hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		//*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		//myCon->Unmap(cBuff, 0);
		//myCon->DrawIndexed(2532, 0, 0);

		//asteroid
		//UINT asteroidstrides[] = { sizeof(_OBJ_VERT_) };
		//UINT asteroidoffsets[] = { 0 };
		//ID3D11Buffer* asteroidmeshVB[] = { asteroidvBuffMesh };
		//myCon->IASetVertexBuffers(0, 1, asteroidmeshVB, asteroidstrides, asteroidoffsets);
		//myCon->IASetIndexBuffer(asteroidiBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		//myCon->VSSetShader(vMeshShader, 0, 0);
		//myCon->PSSetShader(pTextShader, nullptr, 0);
		//myCon->PSSetShaderResources(0, 1, &asteroidtexture);
		//myCon->PSSetSamplers(0, 1, &sampState);
		//myCon->IASetInputLayout(vMeshLayout);
		//temp = XMMatrixIdentity();
		//static float rot = 0; rot += 0.05f;
		//temp = XMMatrixTranslation(0, 0, 0);
		//XMMATRIX temp2 = XMMatrixRotationY(rot);
		//temp = XMMatrixMultiply(temp2, temp);
		//temp.r[3] = XMVectorSetX(temp.r[3], cos(rot) * 5.0f);
		//temp.r[3] = XMVectorSetZ(temp.r[3], sin(rot) * 5.0f);
		//XMStoreFloat4x4(&MyMatricies.wMatrix, temp);
		//hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		//*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		//myCon->Unmap(cBuff, 0);
		//myCon->DrawIndexed(2616, 0, 0);

		//sphere
		UINT spherestrides[] = { sizeof(Vertex) };
		UINT spheredoffsets[] = { 0 };
		ID3D11Buffer* spheremeshVB[] = { spherevBuffMesh };
		myCon->IASetVertexBuffers(0, 1, spheremeshVB, spherestrides, spheredoffsets);
		myCon->IASetIndexBuffer(sphereiBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->PSSetShader(EmissivePS, nullptr, 0);
		myCon->PSSetShaderResources(0, 1, &spheretexture);
		myCon->PSSetSamplers(0, 1, &sampState);
		myCon->IASetInputLayout(vMeshLayout);
		temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(0, 0, 0);
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);
		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);
		myCon->Draw(Sphere.size(), 0);

		mySwap->Present(1, 0);
	}

	//release all D3D11 interfaces
	CleanUp();

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	RECT myWinR;
	GetClientRect(hWnd, &myWinR);

	//attatch d3d11 to our window
	D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swap;
	ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
	swap.BufferCount = 1;
	swap.OutputWindow = hWnd;
	swap.Windowed = true;
	swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferDesc.Width = myWinR.right - myWinR.left;
	swap.BufferDesc.Height = myWinR.bottom - myWinR.top;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.SampleDesc.Count = 1;
	aspectRatio = swap.BufferDesc.Width / float(swap.BufferDesc.Height);
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &dx11, 1, D3D11_SDK_VERSION, &swap, &mySwap, &myDev, 0, &myCon);

	ID3D11Resource* backbuffer;
	hr = mySwap->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);
	hr = myDev->CreateRenderTargetView(backbuffer, NULL, &myRtv);

	backbuffer->Release();

	myPort.Width = swap.BufferDesc.Width;
	myPort.Height = swap.BufferDesc.Height;
	myPort.TopLeftX = myPort.TopLeftY = 0;
	myPort.MinDepth = 0;
	myPort.MaxDepth = 1;

	HUD MyHUD[] = //NDC
	{//xyzw, rgba
		{{-1.0f, 1.0f, 0, 1}, {1, 0, 0, 1}},
		{{-0.5f, 1.0f, 0, 1}, {1, 0, 0, 1}},
		{{-1.0f, 0.5f, 0, 1}, {1, 0, 0, 1}},

		{{-0.5f, 1.0f, 0, 1}, {0, 0, 1, 1}},
		{{-0.5f, 0.5f, 0, 1}, {0, 0, 1, 1}},
		{{-1.0f, 0.5f, 0, 1}, {0, 0, 1, 1}}
	};
	//numVerts = ARRAYSIZE(MyHUD);

	//load it on card

	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(HUD) * 6;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = MyHUD;
	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuff);
	//write and compile & load  basic shaders
	hr = myDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
	hr = myDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);
	//describe it to D3D11
	D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = myDev->CreateInputLayout(ieDesc, 2, VertexShader, sizeof(VertexShader), &vLayout);

	////create constant buffer
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	hr = myDev->CreateBuffer(&bDesc, nullptr, &cBuff);

	//ship
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(SS1_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = SS1_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuffMesh); //vertex mesh buffer
	//bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bDesc.ByteWidth = sizeof(XMFLOAT4) * 2;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//bDesc.MiscFlags = 0;
	//bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	//hr = myDev->CreateBuffer(&bDesc, &subData, &directionallight); 
	//bDesc.ByteWidth = sizeof(XMFLOAT4) * 5;
	//hr = myDev->CreateBuffer(&bDesc, &subData, &pointlight);
	//bDesc.ByteWidth = sizeof(XMFLOAT4) * 7;
	//hr = myDev->CreateBuffer(&bDesc, &subData, &spotlight);
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(SS1_indicies);
	subData.pSysMem = SS1_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &iBuffMesh);
	hr = myDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vMeshShader);
	hr = myDev->CreatePixelShader(TexturePixelShader, sizeof(TexturePixelShader), nullptr, &pTextShader);
	hr = CreateDDSTextureFromFile(myDev, L"HULL.dds", nullptr, &texture);

	//Asteroid
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(asteroid_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = asteroid_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &asteroidvBuffMesh);
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(asteroid_indicies);
	subData.pSysMem = asteroid_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &asteroidiBuffMesh);
	hr = myDev->CreatePixelShader(EmissivePixelShader, sizeof(EmissivePixelShader), nullptr, &EmissivePS);
	hr = CreateDDSTextureFromFile(myDev, L"rock.dds", nullptr, &asteroidtexture);

	//OBJloader
	bool test = LoadOBJ("Assets/Skysphere.obj", Sphere);
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(Vertex) * Sphere.size();
	bDesc.CPUAccessFlags = 0;
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	subData.pSysMem = Sphere.data();
	hr = myDev->CreateBuffer(&bDesc, &subData, &spherevBuffMesh);
	hr = CreateDDSTextureFromFile(myDev, L"Sun.dds", nullptr, &spheretexture);

	//Skybox
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(SkyBoxCube_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = SkyBoxCube_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &skyboxvBuffMesh);
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(SkyBoxCube_indicies);
	subData.pSysMem = SkyBoxCube_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &skyboxiBuffMesh);
	hr = myDev->CreateVertexShader(SkyBoxVS, sizeof(SkyBoxVS), nullptr, &skyBoxVS);
	hr = myDev->CreatePixelShader(SkyBoxPS, sizeof(SkyBoxPS), nullptr, &skyBoxPS);
	hr = CreateDDSTextureFromFile(myDev, L"SpaceSkyBox.dds", nullptr, &skyboxtexture);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = myDev->CreateSamplerState(&sampDesc, &sampState);

	//make matching input layout for mesh vertex
	D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = myDev->CreateInputLayout(meshInputDesc, 3, VertexMeshShader, sizeof(VertexMeshShader), &vMeshLayout);

	//create Z buffer and view
	D3D11_TEXTURE2D_DESC zDesc;
	ZeroMemory(&zDesc, sizeof(zDesc));
	zDesc.ArraySize = 1;
	zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zDesc.Width = swap.BufferDesc.Width;
	zDesc.Height = swap.BufferDesc.Height;
	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.Usage = D3D11_USAGE_DEFAULT;
	zDesc.MipLevels = 1;
	zDesc.SampleDesc.Count = 1;
	hr = myDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);
	hr = myDev->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void CleanUp()
{
	myDev->Release();
	mySwap->Release();
	myCon->Release();
	myRtv->Release();
	vBuff->Release();
	vLayout->Release();
	vShader->Release();
	pShader->Release();
	cBuff->Release();
	vBuffMesh->Release();
	skyboxvBuffMesh->Release();
	asteroidvBuffMesh->Release();
	spherevBuffMesh->Release();
	//directionallight->Release();
	//pointlight->Release();
	//spotlight->Release();
	iBuffMesh->Release();
	skyboxiBuffMesh->Release();
	asteroidiBuffMesh->Release();
	sphereiBuffMesh->Release();
	vMeshShader->Release();
	vMeshLayout->Release();
	pTextShader->Release();
	zBuffer->Release();
	zBufferView->Release();
	texture->Release();
	skyboxtexture->Release();
	asteroidtexture->Release();
	spheretexture->Release();
	sampState->Release();
	skyBoxPS->Release();
	skyBoxVS->Release();
	EmissivePS->Release();
}

bool LoadOBJ(const char* path, vector<Vertex>& OutVector)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector < VEC_3D > temppos;
	std::vector < VEC_3D > tempnrm;
	std::vector < VEC_2D > tempuvw;
	vector<VEC_3D> pos, nrm;
	vector<VEC_2D> uvw;

	FILE* filepoint;
	errno_t err;
	err = fopen_s(&filepoint, path, "r");

	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(filepoint, "%s", lineHeader, 128);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0)
		{
			VEC_3D vertex;
			fscanf_s(filepoint, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temppos.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			VEC_2D uv;
			fscanf_s(filepoint, " %f %f\n", &uv.x, &uv.y);
			tempuvw.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			VEC_3D normal;
			fscanf_s(filepoint, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tempnrm.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(filepoint, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		VEC_3D vertex = temppos[vertexIndex - 1];
		pos.push_back(vertex);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int uvIndex = uvIndices[i];
		VEC_2D uv = tempuvw[uvIndex - 1];
		uvw.push_back(uv);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		unsigned int normalIndex = normalIndices[i];
		VEC_3D normal = tempnrm[normalIndex - 1];
		nrm.push_back(normal);
	}
	vector<Vertex> vertexList;
	for (unsigned i = 0; i < vertexIndices.size(); i++)
	{
		Vertex V;
		V.pos = pos[i];
		V.normal = nrm[i];
		V.texCoord = uvw[i];
		OutVector.push_back(V);
		vertexList.push_back(V);
	}

	vector<int> IL;
	int compactedindex = 0;
	for (int j = 0; j < vertexList.size(); j++)
	{
		bool found = false;
		for (int i = 0; i < OutVector.size(); i++)
		{
			if (vertexList[j].pos.x      ==		OutVector[i].pos.x &&
				vertexList[j].pos.y      ==		OutVector[i].pos.y &&
				vertexList[j].pos.z      ==		OutVector[i].pos.z &&
				vertexList[j].normal.x   ==		OutVector[i].normal.x &&
				vertexList[j].normal.y   ==		OutVector[i].normal.y &&
				vertexList[j].normal.z   ==		OutVector[i].normal.z &&
				vertexList[j].texCoord.x ==		OutVector[i].texCoord.x &&
				vertexList[j].texCoord.y ==		OutVector[i].texCoord.y )
			{
				IL.push_back(i);
				found = true;
				break;
			}
		}
		if (!found)
		{
			OutVector.push_back(vertexList[j]);
			IL.push_back(compactedindex);
			compactedindex++;
		}
	}

	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(Vertex) * Sphere.size();
	subData.pSysMem = IL.data();
	hr = myDev->CreateBuffer(&bDesc, &subData, &sphereiBuffMesh);
}