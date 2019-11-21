#include "FirstPersonPlayer.h"
#include "GameObject.h"
#include "Input.h"
#include "Direct3D.h"
#include <xnamath.h>
#include "Camera.h"

void FirstPersonPlayer::Start()
{
}

void FirstPersonPlayer::Update()
{
	if (!m_pPhysComp)
	{
		m_pPhysComp = GetOwner()->GetComponent<PhysicsComponent>();
	}
	else
	{
		int moveSpeed = 5;
		XMFLOAT2 mouse = Input::GetInstance()->GetMouseDelta();

		mouse.x *= m_mouseSens;
		mouse.y *= m_mouseSens;

		XMFLOAT3 newRot = m_owner->GetWorldRotation();
		newRot.y += mouse.x;
		m_owner->SetRotation(newRot);

		XMFLOAT3 newVel = { 0,0,0 };

		if (Input::GetInstance()->IsKeyPressed(DIK_W))
		{
			XMFLOAT3 forward = m_owner->GetForward();
			//forward.y = 0;
			newVel.x += forward.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
			newVel.z += forward.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		}
		else if (Input::GetInstance()->IsKeyPressed(DIK_S))
		{
			XMFLOAT3 forward = m_owner->GetForward();
			newVel.x += -forward.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
			newVel.z += -forward.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		}

		if (Input::GetInstance()->IsKeyPressed(DIK_D))
		{
			XMFLOAT3 right = m_owner->GetRight();
			newVel.x += right.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;
			newVel.z += right.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;
		}
		else if (Input::GetInstance()->IsKeyPressed(DIK_A))
		{
			XMFLOAT3 right = m_owner->GetRight();
			newVel.x += -right.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
			newVel.z += -right.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		}

		XMFLOAT3 physVel = m_pPhysComp->GetVelocity();
		newVel.y = physVel.y;
		m_pPhysComp->SetVelocity(newVel);

		XMFLOAT3 cameraRot = Camera::GetMain()->GetLocalRotation();
		cameraRot.x += mouse.y;
		Camera::GetMain()->SetRotation(cameraRot);
	}
}
