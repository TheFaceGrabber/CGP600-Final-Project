#pragma once
#include "Component.h"
#include "PhysicsComponent.h"
#include "Mesh.h"

class FirstPersonPlayer : public Component
{
private:
	float m_maxCameraPitch = 80;
	float m_minCameraPitch = -80;
	float m_mouseSens = 6;
	float m_fireRate = .5f;
	float m_lastFireTime;
	Mesh* m_bulletMesh;

	int m_pigeonCount;
	PhysicsComponent* m_pPhysComp;
public:
	void Start() override;
	void Update() override;

	void AddPigeon();
	void RemovedPigeon();
};

