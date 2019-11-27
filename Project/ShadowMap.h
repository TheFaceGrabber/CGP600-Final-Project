#pragma once
#include <d3d11.h>
#include <dxgi.h>

class ShadowMap
{
private:
	const int MapSize = 2048;

	ID3D11ShaderResourceView* m_pShadowMap;
	ID3D11DepthStencilView* m_pZShadowBuffer;
	D3D11_VIEWPORT				shadowViewport;
	ID3D11RasterizerState* m_pShadowRenderState;

public:
	ShadowMap();
	~ShadowMap();

	void ClearShadowMap();
	void ApplyToShaders();
};

