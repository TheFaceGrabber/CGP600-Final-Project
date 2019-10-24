#include "MeshRenderer.h"
#include "Direct3D.h"
#include "GameObject.h"
#include "Camera.h"
#include "ConstantBuffers.h"

MeshRenderer::~MeshRenderer()
{
	delete m_pMesh;
}

void MeshRenderer::UpdateGfx()
{
	TransformationBuffer m_transBuffer;
	m_transBuffer.World = m_owner->GetWorldMatrix();
	m_transBuffer.WorldViewProjection = m_owner->GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix();

	ConstantBuffers::GetInstance()->Bind(BUFFER_TRANSFORMATIONS, &m_transBuffer);
	m_pMesh->Draw();
}
