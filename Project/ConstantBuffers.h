#pragma once
#include <d3d11.h>
#include <xnamath.h>
#include <vector>
#include "Shader.h"

#define BUFFER_COUNT 3
#define BUFFER_LIGHTING 0
#define BUFFER_TRANSFORMATIONS 1
#define BUFFER_SHADER_PROPERTIES 2
#define BUFFER_SHADER_PROPERTIES_MAX_SIZE 128


struct LightingBuffer //BUFFER 0
{
	XMFLOAT3 LightVector;
	float LightIntensity;
	XMFLOAT4 LightColour;
	XMFLOAT4 AmbientColour;
	XMFLOAT3 CameraPosition;
	float padding;
}; 

struct TransformationBuffer //BUFFER 1
{
	XMMATRIX WorldViewProjection;
	XMMATRIX World;
	XMMATRIX DirLightView;
	XMMATRIX DirLightProjection;
};

class ConstantBuffers
{
private:
	ID3D11Buffer* m_pBuffers[BUFFER_COUNT];

	static ConstantBuffers* m_pInstance;
public:

	ConstantBuffers();
	~ConstantBuffers();

	void Bind(int index, void* data);
	static ConstantBuffers* GetInstance();
	static void Release()
	{
		delete m_pInstance;
	}
};


