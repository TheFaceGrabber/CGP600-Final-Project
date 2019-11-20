#include "ShadowMap.h"
#include "Direct3D.h"

ShadowMap::ShadowMap()
{
	//TODO move this all to a class
	//Create shadowmap
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = MapSize;
	shadowMapDesc.Width = MapSize;

	ID3D11Texture2D* pShadowMap;
	Direct3D::GetInstance()->GetDevice()->CreateTexture2D(&shadowMapDesc, nullptr, &pShadowMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	Direct3D::GetInstance()->GetDevice()->CreateDepthStencilView(
		pShadowMap,
		&depthStencilViewDesc,
		&m_pZShadowBuffer
	);

	Direct3D::GetInstance()->GetDevice()->CreateShaderResourceView(
		pShadowMap,
		&shaderResourceViewDesc,
		&m_pShadowMap
	);
	pShadowMap->Release();

	ZeroMemory(&shadowViewport, sizeof(D3D11_VIEWPORT));
	shadowViewport.Width = (FLOAT)MapSize;
	shadowViewport.Height = (FLOAT)MapSize;
	shadowViewport.MinDepth = 0.0f;
	shadowViewport.MaxDepth = 1.0f;

	D3D11_RASTERIZER_DESC shadowRenderStateDesc;
	ZeroMemory(&shadowRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;

	Direct3D::GetInstance()->GetDevice()->CreateRasterizerState(
		&shadowRenderStateDesc,
		&m_pShadowRenderState
	);
	//End create shadowmap
}

ShadowMap::~ShadowMap()
{
	if(m_pShadowMap)
		m_pShadowMap->Release();

	if(m_pZShadowBuffer)
		m_pZShadowBuffer->Release();

	if(m_pShadowRenderState)
		m_pShadowRenderState->Release();
}

void ShadowMap::ClearShadowMap()
{
	float rgba_clear_colour[4] = { 0.4, 0.58, 0.92, 1.0f };
	Direct3D::GetInstance()->GetContext()->ClearRenderTargetView(Direct3D::GetInstance()->GetBackBufferTargetView(), rgba_clear_colour);
	Direct3D::GetInstance()->GetContext()->ClearDepthStencilView(m_pZShadowBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Direct3D::GetInstance()->GetContext()->OMSetRenderTargets(0, nullptr, m_pZShadowBuffer);
	Direct3D::GetInstance()->GetContext()->RSSetViewports(1, &shadowViewport);

	Direct3D::GetInstance()->GetContext()->RSSetState(m_pShadowRenderState);

}

void ShadowMap::ApplyToShaders()
{
	Direct3D::GetInstance()->GetContext()->PSSetShaderResources(0, 1, &m_pShadowMap);
}
