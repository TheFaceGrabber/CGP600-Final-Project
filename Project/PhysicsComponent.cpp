#include "PhysicsComponent.h"
#include "GameObject.h"
#include "Direct3D.h"

void PhysicsComponent::SetVelocity(XMFLOAT3 vel)
{
	m_Velocity = vel;
}

XMFLOAT3 PhysicsComponent::GetVelocity()
{
	return m_Velocity;
}

void PhysicsComponent::SetUseGravity(bool use)
{
	m_UseGravity = use;
}

bool PhysicsComponent::GetUseGravity()
{
	return m_UseGravity;
}

void PhysicsComponent::Update()
{
	XMFLOAT3 curPos = GetOwner()->GetPosition();
	curPos.x += m_Velocity.x;
	curPos.y += m_Velocity.y;
	curPos.z += m_Velocity.z;

	GetOwner()->SetPosition(curPos);

	if (m_UseGravity)
		m_Velocity.y += (GRAVITY * Direct3D::GetInstance()->GetDeltaTime());
}
