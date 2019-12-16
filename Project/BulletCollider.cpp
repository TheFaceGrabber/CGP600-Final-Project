#include "BulletCollider.h"
#include "Direct3D.h"
#include "Scene.h"
#include "PigeonComponent.h"

void BulletCollider::Update()
{
	if (GetOwner()) {
		XMFLOAT3 center = GetOwner()->GetWorldPosition();
		center.x += GetCentre().x;
		center.y += GetCentre().y;
		center.z += GetCentre().z;
		GameObject* obj = Direct3D::GetInstance()->GetCurrentScene()->CheckForObjectCollision(GetOwner(), center, GetWidth(), GetHeight(), true);
		if (obj != nullptr) 
		{
			Collided(obj);
			return;
		}
	}
	
	BoundingBoxCollider::Update();
}

void BulletCollider::Collided(GameObject* obj)
{
	if(obj != nullptr)
	{
		PigeonComponent* pigeon = obj->GetComponent<PigeonComponent>();
		OutputDebugStringA(("Bullet collided with!" + obj->GetName() + "\n").c_str());
		if (pigeon != nullptr)
		{
			pigeon->Died();
			Direct3D::GetInstance()->GetCurrentScene()->DestroyGameObject(obj);
		}
	}
	else
	{
		XMFLOAT3 pos = GetOwner()->GetWorldPosition();
		OutputDebugStringA(("Bullet collided at " + std::to_string(pos.x) + ", "+ std::to_string(pos.y) + ", "+ std::to_string(pos.z) +"!\n").c_str());
	}
	Direct3D::GetInstance()->GetCurrentScene()->DestroyGameObject(GetOwner());
}
