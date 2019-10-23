#pragma once
#include <d3d11.h>
#include <dxgi.h>

#define UP XMVECTOR{0,1,0}
#define RIGHT XMVECTOR{1,0,0}
#define FORWARD XMVECTOR{0,0,1}

#include "Mesh.h"

class Direct3D
{
private:
	D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*			g_pD3DDevice = NULL;
	ID3D11DeviceContext*	g_pImmediateContext = NULL;
	IDXGISwapChain*			g_pSwapChain = NULL;

	ID3D11RenderTargetView* g_pBackBufferRTView = NULL;

	//ID3D11Buffer*			g_pVertexBuffer;
	//ID3D11VertexShader*		g_pVertexShader;
	//ID3D11PixelShader*		g_pPixelShader;
	//ID3D11InputLayout*		g_pInputLayout;

	ID3D11DepthStencilView* g_pZBuffer;

	double					m_lastTimeSample = 0;

	double					m_time = 0;
	double					m_deltaTime = 0;
	double					m_fps = 0;
	int						m_screen_width;
	int						m_screen_height;

	static Direct3D*		m_pInstance;
	HWND m_hWnd;
	HINSTANCE m_hInst;

public:
	Direct3D();
	~Direct3D();

	static Direct3D* GetInstance();
	static void Release()
	{
		delete m_pInstance;
	}

	HRESULT InitialiseD3D(HWND hWnd, HINSTANCE hInst);

	HINSTANCE GetHInstance() { return m_hInst; }
	HWND GetHWindow() { return m_hWnd; }

	void	Draw(Mesh* mesh);

	int		GetScreenWidth() { return m_screen_width; }
	int		GetScreenHeight() { return m_screen_height; }

	float	GetAspectRatio() { return (float)m_screen_width / (float)m_screen_height; }

	float	GetDeltaTime() { return m_deltaTime; }
	float	GetTime() { return m_time; }

	void	ReleaseD3D();
	void	RunUpdate();
	void	UpdateWindow(int w, int h);

	ID3D11Device* GetDevice() { return g_pD3DDevice; }
	ID3D11DeviceContext* GetContext() { return g_pImmediateContext; }
};

