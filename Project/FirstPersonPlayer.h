#pragma once
#include "Component.h"
#include "PhysicsComponent.h"

class FirstPersonPlayer : public Component
{
private:
	float m_maxCameraPitch = 80;
	float m_minCameraPitch = -80;
	float m_mouseSens = 6;
	PhysicsComponent* m_pPhysComp;
public:
	void Start() override;
	void Update() override;
};

