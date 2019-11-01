#include "FlyCamera.h"
#include "Input.h"
#include "GameObject.h"
#include "Direct3D.h"
#include <xnamath.h>
#include "Camera.h"
#include "DirectionLight.h"

void FlyCamera::Start()
{
	Camera::SetMain(m_owner);

	m_owner->SetPosition({ 0,0,-1 });
}

void FlyCamera::Update()
{
	int moveSpeed = 10;
	XMFLOAT2 mouse = Input::GetInstance()->GetMouseDelta();

	mouse.x *= m_mouseSens;
	mouse.y *= m_mouseSens;

	XMFLOAT3 newRot = m_owner->GetRotation();
	newRot.x += mouse.y;
	newRot.y += mouse.x;
	m_owner->SetRotation(newRot);

	XMFLOAT3 newPos = m_owner->GetPosition();

	if (Input::GetInstance()->IsMouseButtonPressed(1))
	{
		Camera::SetFov(15);
	}
	else
	{
		Camera::SetFov(65);
	}

	if (Input::GetInstance()->IsKeyPressed(DIK_W))
	{
		XMFLOAT3 forward = m_owner->GetForward();
		newPos.x += forward.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		newPos.y += forward.y * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		newPos.z += forward.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();

		m_owner->SetPosition(newPos);
	}
	if (Input::GetInstance()->IsKeyPressed(DIK_S))
	{
		XMFLOAT3 forward = m_owner->GetForward();
		newPos.x -= forward.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		newPos.y -= forward.y * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();
		newPos.z -= forward.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();

		m_owner->SetPosition(newPos);
	}

	if (Input::GetInstance()->IsKeyPressed(DIK_D))
	{
		XMFLOAT3 right = m_owner->GetRight();
		newPos.x += right.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;
		newPos.y += right.y * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;
		newPos.z += right.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;

		m_owner->SetPosition(newPos);
	}
	if (Input::GetInstance()->IsKeyPressed(DIK_A))
	{
		XMFLOAT3 right = m_owner->GetRight();
		newPos.x -= right.x * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;
		newPos.y -= right.y * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;
		newPos.z -= right.z * moveSpeed * Direct3D::GetInstance()->GetDeltaTime();;

		m_owner->SetPosition(newPos);
	}
}
