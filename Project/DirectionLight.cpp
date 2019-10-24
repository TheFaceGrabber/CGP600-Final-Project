#include "DirectionLight.h"

DirectionLight* DirectionLight::m_pMain = nullptr;


void DirectionLight::Start()
{
	if (m_pMain == nullptr)
		m_pMain = this;
}

DirectionLight* DirectionLight::GetMainLight()
{
	if (m_pMain != nullptr)
		return m_pMain;

	return nullptr;
}

void DirectionLight::SetColour(XMFLOAT4 col)
{
	m_Colour = col;
}

void DirectionLight::SetAmbientColour(XMFLOAT4 col)
{
	m_AmbColour = col;
}

void DirectionLight::SetIntensity(float intensity)
{
	m_Intensity = intensity;
}
