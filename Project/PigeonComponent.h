#pragma once
#include "Component.h"
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>
#include <vector>

class PigeonComponent : public Component
{
private:
	float radiansToDegrees = 57.29578f;

	const int ATTACKDISTANCE = 15;
	float m_distanceToPlayer = 0;
	GameObject* m_pPlayer = nullptr;
	XMFLOAT3 m_lastPlayerPos;
	std::vector<XMFLOAT3> m_curPath;
public:
	void SetPlayer(GameObject* player);
	void Died();
	
	void Update() override;
};

