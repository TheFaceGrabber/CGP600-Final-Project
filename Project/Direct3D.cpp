#include "Direct3D.h"
#include <cmath>
#include <iostream>
#include "ConstantBuffers.h"
#include <chrono>
#include <string>
#include "Input.h"
#include "GameObject.h"
#include "FlyCamera.h"
#include <d3dx11.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "Camera.h"
#include "DirectionLight.h"
#include "Scene.h"
#include "Skybox.h"
#include "GUI.h"

//Double ended queue for particles

using namespace std::chrono;

Direct3D* Direct3D::m_pInstance = NULL;

//Mesh* m;

Direct3D::Direct3D()
{
}

Direct3D::~Direct3D()
{
	//delete m;
	UnloadCurrentScene();
	delete m_pBasicShader;
	delete m_pShadowMap;
	ConstantBuffers::Release();
	Input::Release();
	GUI::Release();
}

#pragma region Initalisers
HRESULT Direct3D::InitialiseD3D(HWND hWnd, HINSTANCE hInst)
{
	m_hWnd = hWnd;
	m_hInst = hInst;

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);
	m_screen_width = rc.right - rc.left;
	m_screen_height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_screen_width;
	sd.BufferDesc.Height = m_screen_height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D* pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)& pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
	tex2dDesc.Width = m_screen_width;
	tex2dDesc.Height = m_screen_height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr))return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();


	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Set the viewport
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)m_screen_width;
	viewport.Height = (FLOAT)m_screen_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC shadowRenderStateDesc;
	ZeroMemory(&shadowRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;

	shadowRenderStateDesc.CullMode = D3D11_CULL_BACK;

	Direct3D::GetInstance()->GetDevice()->CreateRasterizerState(
		&shadowRenderStateDesc,
		&m_pDefaultRenderState
	);

	m_pShadowMap = new ShadowMap();

	m_lastTimeSample = 0;

	m_pBasicShader = new Shader("Unlit.hlsl");

	LoadScene("Test");

	Input::GetInstance()->Update();
	return S_OK;
}
#pragma endregion

#pragma region Updaters
void Direct3D::RunUpdate()
{
	Input::GetInstance()->Update();

	if (Input::GetInstance()->IsKeyPressed(DIK_ESCAPE))
		DestroyWindow(m_hWnd);

	LightingBuffer lightBuff;

	//Sleep(1.0f);
	double t = (double)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() / 1000.0f;

	double diff = t - m_lastTimeSample;

	//If first frame, make sure delta time is 0 otherwise gravity will break
	if (m_lastTimeSample != 0) 
	{
		m_deltaTime = diff;
	}
	else 
	{
		m_deltaTime = 0;
	}

	m_lastTimeSample = t;
	m_time += m_deltaTime;
	m_fps = 1.0f / m_deltaTime;

	m_pScene->Update();
	float rgba_clear_colour[4] = { 0.4, 0.58, 0.92, 1.0f };

	m_pShadowMap->ClearShadowMap();

	m_pScene->UpdateShadowMap();

	g_pImmediateContext->RSSetState(m_pDefaultRenderState);
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);
	g_pImmediateContext->RSSetViewports(1, &viewport);
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//GUI::GetInstance()->DrawGUIText(std::to_string(round((1 / m_deltaTime) * 2.f) / 2.f), -1, 1, .05);

	if(Camera::GetMain())
		lightBuff.CameraPosition = Camera::GetMain()->GetWorldPosition();

	if (DirectionLight::GetMainLight()) 
	{
		lightBuff.LightVector = DirectionLight::GetMainLight()->GetOwner()->GetForward();
		lightBuff.LightColour = DirectionLight::GetMainLight()->GetColour();
		lightBuff.LightIntensity = DirectionLight::GetMainLight()->GetIntensity();
		lightBuff.AmbientColour = DirectionLight::GetMainLight()->GetAmbientColour();
	}
	ConstantBuffers::GetInstance()->Bind(BUFFER_LIGHTING, &lightBuff);

	m_pScene->UpdateGfx();
	GUI::GetInstance()->UpdateGfx();
	m_pScene->UpdateDestruction();
	g_pSwapChain->Present(0, 0);
}

void Direct3D::Draw(Mesh* mesh)
{
	m_pShadowMap->ApplyToShaders();

	g_pImmediateContext->VSSetShader(mesh->GetShader()->GetVertexShader(), 0, 0);
	g_pImmediateContext->PSSetShader(mesh->GetShader()->GetPixelShader(), 0, 0);

	ID3D11Buffer* vertBuff = mesh->GetVertexBuffer();
	long vertSize = sizeof(DefaultVertex);
	UINT stride = vertSize;
	UINT offset = 0;

	g_pImmediateContext->IASetInputLayout(mesh->GetShader()->GetInputLayout());
	g_pImmediateContext->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pImmediateContext->Draw(mesh->GetVertices().capacity(), 0);
}

void Direct3D::DrawBasic(Mesh* mesh)
{
	g_pImmediateContext->VSSetShader(m_pBasicShader->GetVertexShader(), 0, 0);
	g_pImmediateContext->PSSetShader(m_pBasicShader->GetPixelShader(), 0, 0);

	ID3D11Buffer* vertBuff = mesh->GetVertexBuffer();
	long vertSize = sizeof(DefaultVertex);
	UINT stride = vertSize;
	UINT offset = 0;

	g_pImmediateContext->IASetInputLayout(m_pBasicShader->GetInputLayout());
	g_pImmediateContext->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pImmediateContext->Draw(mesh->GetVertices().capacity(), 0);
}

void Direct3D::UpdateWindow(int w, int h)
{
	if (!g_pSwapChain)
		return;

	m_screen_width = w;
	m_screen_height = h;

	g_pImmediateContext->OMSetRenderTargets(0, 0, 0);

	g_pBackBufferRTView->Release();

	HRESULT hr;

	hr = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* pBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& pBuffer);

	hr = g_pD3DDevice->CreateRenderTargetView(pBuffer, NULL, &g_pBackBufferRTView);

	pBuffer->Release();

	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
	tex2dDesc.Width = m_screen_width;
	tex2dDesc.Height = m_screen_height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	viewport.Width = w;
	viewport.Height = h;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

void Direct3D::LoadScene(std::string sceneLoc)
{
	UnloadCurrentScene();


	m_pScene = Scene::LoadFromFile("Assets/Levels/" + sceneLoc + ".jscene");
}

void Direct3D::UnloadCurrentScene()
{
	if(m_pScene)
		delete m_pScene;
}

Scene* Direct3D::GetCurrentScene()
{
	return m_pScene;
}

#pragma endregion

Direct3D* Direct3D::GetInstance()
{
	if (!m_pInstance) 
	{
		m_pInstance = new Direct3D();
	}
	return m_pInstance;
}

void Direct3D::ReleaseD3D()
{
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();

	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();

	if (g_pZBuffer) g_pZBuffer->Release();

	m_pDefaultRenderState->Release();
}
