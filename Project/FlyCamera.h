#pragma once
#include "Component.h"

class FlyCamera : public Component
{
private:
	float m_mouseSens = 6;
public:
	void Start() override;
	void Update() override;
};

