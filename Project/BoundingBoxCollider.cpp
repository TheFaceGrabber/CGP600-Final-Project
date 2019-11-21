#include "BoundingBoxCollider.h"
#include "GameObject.h"
#include "Direct3D.h"
#include "Scene.h"

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
		XMFLOAT3 colliderCentrePos = {GetOwner()->GetWorldPosition().x + m_Centre.x, GetOwner()->GetWorldPosition().y + m_Centre.y, GetOwner()->GetWorldPosition().z + m_Centre.z };
		XMFLOAT3 startingVelocity = m_pPhysComp->GetVelocity();

#pragma region CheckDown
		if( Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x - m_Width, (colliderCentrePos.y - m_Height), colliderCentrePos.z - m_Width })||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x + m_Width, (colliderCentrePos.y - m_Height), colliderCentrePos.z - m_Width })||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x + m_Width, (colliderCentrePos.y - m_Height), colliderCentrePos.z + m_Width })||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x - m_Width, (colliderCentrePos.y - m_Height), colliderCentrePos.z + m_Width }))
		{
			startingVelocity.y = 0;
		}
#pragma  endregion 

#pragma region Front
		if (startingVelocity.z > 0 &&
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x + m_Width, (colliderCentrePos.y - m_Height + 0.05f),	colliderCentrePos.z + (m_Width+.05f) + startingVelocity.z}) ||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x - m_Width, (colliderCentrePos.y + m_Height),	colliderCentrePos.z + (m_Width+.05f) + startingVelocity.z }))
		{
			startingVelocity.z = 0;
		}
#pragma  endregion 

#pragma region Back
		if (startingVelocity.z < 0 && 
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x + m_Width, (colliderCentrePos.y),				colliderCentrePos.z - (m_Width - .05f) + startingVelocity.z }) ||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x - m_Width, (colliderCentrePos.y + m_Height),	colliderCentrePos.z - (m_Width - .05f) + startingVelocity.z }))
		{
			startingVelocity.z = 0;
		}
#pragma  endregion 

#pragma region Left
		if (Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x - (m_Width + 0.05f) + startingVelocity.x, (colliderCentrePos.y),			colliderCentrePos.z + (m_Width + 0.05f) }) ||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x - (m_Width + 0.05f) + startingVelocity.x, (colliderCentrePos.y + m_Height),	colliderCentrePos.z - (m_Width - 0.05f) }))
		{
			if(startingVelocity.x < 0)
				startingVelocity.x = 0;
		}
#pragma  endregion 

#pragma region Right
		if (Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x + (m_Width + 0.05f) + startingVelocity.x, (colliderCentrePos.y),			colliderCentrePos.z + (m_Width + 0.05f) }) ||
			Direct3D::GetInstance()->GetCurrentScene()->CheckForVoxel({ colliderCentrePos.x + (m_Width + 0.05f) + startingVelocity.x, (colliderCentrePos.y + m_Height),	colliderCentrePos.z - (m_Width - 0.05f) }))
		{
			if (startingVelocity.x > 0)
				startingVelocity.x = 0;
		}
#pragma  endregion 

		m_pPhysComp->SetVelocity(startingVelocity);
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
