#pragma once
#include "Component.h"
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>

class DirectionLight : public Component
{
private:
	static DirectionLight* m_pMain;

	float m_Intensity;
	XMFLOAT4 m_Colour;
	XMFLOAT4 m_AmbColour;
public:
	void Start() override;
	static DirectionLight* GetMainLight();

	void SetColour(XMFLOAT4 col);
	void SetAmbientColour(XMFLOAT4 col);
	void SetIntensity(float intensity);

	int GetIntensity() { return m_Intensity; }
	XMFLOAT4 GetColour() { return m_Colour; }
	XMFLOAT4 GetAmbientColour() { return m_AmbColour; }
};

