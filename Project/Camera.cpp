#include "Camera.h"
#include "Direct3D.h"

GameObject* Camera::m_pMain = nullptr;
float Camera::m_fov = 65;
float Camera::m_farPlane = 100;
float Camera::m_closePlane = .1;

GameObject* Camera::GetMain()
{
	return m_pMain;
}

float Camera::GetFov()
{
	return m_fov;
}

float Camera::GetFarPlane()
{
	return m_farPlane;
}

void Camera::SetMain(GameObject* cam)
{
	if (m_pMain == nullptr)
		m_pMain = cam;
}

void Camera::SetFov(float fov)
{
	m_fov = fov;
}

XMMATRIX Camera::GetViewMatrix()
{
	XMVECTOR pos = { m_pMain->GetWorldPosition().x,m_pMain->GetWorldPosition().y,m_pMain->GetWorldPosition().z };
	XMVECTOR forward = { m_pMain->GetForward().x,m_pMain->GetForward().y,m_pMain->GetForward().z };
	XMVECTOR up = { m_pMain->GetUp().x,m_pMain->GetUp().y,m_pMain->GetUp().z };
	return XMMatrixLookToLH(pos, forward, up);
}

XMMATRIX Camera::GetProjectionMatrix()
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fov), Direct3D::GetInstance()->GetAspectRatio(), m_closePlane, m_farPlane);
}
