#pragma once
#include "PhysicsComponent.h"
#include "Component.h"
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>

class BoundingBoxCollider : public Component
{
protected:
	bool m_didCollide;
	GameObject* m_collidedWith;
	
private:
	PhysicsComponent* m_pPhysComp;
	float m_Height;
	float m_Width;
	XMFLOAT3 m_Centre;

public:
	void Start() override;
	void Update() override;

	virtual void Collided(GameObject* obj);

	void SetHeight(float h);
	void SetWidth(float w);
	void SetCentre(XMFLOAT3 centre);

	float GetHeight();
	float GetWidth();
	XMFLOAT3 GetCentre();
};

