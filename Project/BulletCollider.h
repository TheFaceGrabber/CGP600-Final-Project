#pragma once
#include "BoundingBoxCollider.h"
class BulletCollider :
	public BoundingBoxCollider
{
public:
	void Update() override;

	void Collided(GameObject* obj) override;
};

