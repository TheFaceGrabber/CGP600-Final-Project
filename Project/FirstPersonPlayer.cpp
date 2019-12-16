#include "FirstPersonPlayer.h"
#include "GameObject.h"
#include "Input.h"
#include "Direct3D.h"
#include <xnamath.h>
#include "Camera.h"
#include "Scene.h"
#include "GUI.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "BoundingBoxCollider.h"
#include "BulletCollider.h"

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

		if (cameraRot.x > m_maxCameraPitch)
			cameraRot.x = m_maxCameraPitch;

		if (cameraRot.x < m_minCameraPitch)
			cameraRot.x = m_minCameraPitch;

		Camera::GetMain()->SetRotation(cameraRot);

		if(Input::GetInstance()->IsMouseButtonPressed(0))
		{
			if (Direct3D::GetInstance()->GetTime() > m_lastFireTime + m_fireRate) 
			{
				if(m_bulletMesh == nullptr)
				{
					m_bulletMesh = new Mesh();
					m_bulletMesh->LoadFromFile("Assets/Models/bullet.obj");
				}
				
				GameObject* bullet = new GameObject("bullet");
				bullet->SetScale({ .5 ,.5 ,.5 });
				bullet->SetPosition(Camera::GetMain()->GetWorldPosition());
				MeshRenderer* render = (MeshRenderer*)bullet->AddComponent(new MeshRenderer());
				render->SetMesh(m_bulletMesh);
				
				BulletCollider* box = (BulletCollider*)bullet->AddComponent(new BulletCollider());
				box->SetWidth(.25);
				box->SetHeight(.25);
				PhysicsComponent* phys = (PhysicsComponent*)bullet->AddComponent(new PhysicsComponent());
				phys->SetUseGravity(false);

				Direct3D::GetInstance()->GetCurrentScene()->RegisterGameObject(bullet);
				
				XMFLOAT3 vel = Camera::GetMain()->GetForward();
				//vel.x *= 0.1f;
				//vel.y *= 0.1f;
				//vel.z *= 0.1f;
				phys->SetVelocity(vel);
				
				m_lastFireTime = Direct3D::GetInstance()->GetTime();
			}
		}
		
		GUI::GetInstance()->DrawGUIText(std::to_string(m_pigeonCount) + " Pigeons Left", -1, -.92, .08);
		GUI::GetInstance()->DrawGUIText("100%", .6, -.9, .1);
		GUI::GetInstance()->DrawGUIText(".", -0.005, 0.005, .01);
	}
}

void FirstPersonPlayer::AddPigeon()
{
	m_pigeonCount++;
}

void FirstPersonPlayer::RemovedPigeon()
{
	m_pigeonCount--;
}
