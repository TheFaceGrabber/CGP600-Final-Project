#include "Skybox.h"
#include "Direct3D.h"
#include "ConstantBuffers.h"
#include "Camera.h"

Skybox::Skybox(std::string skyboxLoc)
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	Direct3D::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_pDefaultDepth);

	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Direct3D::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_pSkyboxDepth);

	m_pShader = new Shader("Skybox.hlsl");
	m_pShader->ApplyTexture(skyboxLoc, 0);

	m_pSkyMesh = new Mesh();
	m_pSkyMesh->LoadFromFile(m_meshLoc);
	m_pSkyMesh->SetShader(m_pShader);
}

Skybox::~Skybox()
{
	if(m_pSkyMesh)
		delete m_pSkyMesh;

	if (m_pSkyboxDepth)
		m_pSkyboxDepth->Release();

	if (m_pDefaultDepth)
		m_pDefaultDepth->Release();
}

void Skybox::UpdateGfx()
{
	Direct3D::GetInstance()->GetContext()->OMSetDepthStencilState(m_pSkyboxDepth, 2);

	TransformationBuffer m_transBuffer;
	XMFLOAT3 camPos = Camera::GetMain()->GetPosition();
	m_transBuffer.World = XMMatrixScaling(Camera::GetFarPlane()/2, Camera::GetFarPlane()/2, Camera::GetFarPlane()/2) * XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
	m_transBuffer.WorldViewProjection = m_transBuffer.World * Camera::GetViewMatrix() * Camera::GetProjectionMatrix();

	ConstantBuffers::GetInstance()->Bind(BUFFER_TRANSFORMATIONS, &m_transBuffer);

	m_pSkyMesh->Draw();

	Direct3D::GetInstance()->GetContext()->OMSetDepthStencilState(m_pDefaultDepth, 1);
}
