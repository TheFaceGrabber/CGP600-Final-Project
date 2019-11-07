#include "BoundingBoxCollider.h"
#include "GameObject.h"

void BoundingBoxCollider::Start()
{
}

void BoundingBoxCollider::Update()
{
	if (!m_pPhysComp)
	{
		m_pPhysComp = GetOwner()->GetComponent<PhysicsComponent>();
	}
	else
	{

	}
}

void BoundingBoxCollider::SetHeight(float h)
{
	m_Height = h;
}

void BoundingBoxCollider::SetWidth(float w)
{
	m_Width = w;
}

void BoundingBoxCollider::SetCentre(XMFLOAT3 centre)
{
	m_Centre = centre;
}

float BoundingBoxCollider::GetHeight()
{
	return m_Height;
}

float BoundingBoxCollider::GetWidth()
{
	return m_Width;
}

XMFLOAT3 BoundingBoxCollider::GetCentre()
{
	return m_Centre;
}
