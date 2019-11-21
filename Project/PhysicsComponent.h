#pragma once
#include "Component.h"
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>

class PhysicsComponent : public Component
{
private:
	const float GRAVITY = -9.81;

	XMFLOAT3 m_Velocity;
	bool m_UseGravity = true;
public:
	void SetVelocity(XMFLOAT3 vel);
	XMFLOAT3 GetVelocity();

	void SetUseGravity(bool use);
	bool GetUseGravity();

	void Update() override;
};

