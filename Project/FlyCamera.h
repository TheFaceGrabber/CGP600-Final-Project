#pragma once
#include "Component.h"
#include "PhysicsComponent.h"

class FlyCamera : public Component
{
private:
	float m_mouseSens = 6;
	PhysicsComponent* m_pPhysComp;
public:
	void Start() override;
	void Update() override;
};

