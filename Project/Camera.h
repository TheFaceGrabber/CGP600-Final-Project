#pragma once
#include "GameObject.h"

class Camera
{
private:
	static GameObject* m_pMain;
	static float m_fov;
	static float m_farPlane;
	static float m_closePlane;
public:
	static GameObject* GetMain();
	static float GetFov();
	static void SetMain(GameObject* cam);
	static void SetFov(float fov);
	static XMMATRIX GetViewMatrix();
	static XMMATRIX GetProjectionMatrix();
};

