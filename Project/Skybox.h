#pragma once
#include <string>
#include "Mesh.h"

class Skybox
{
private:
	std::string m_meshLoc = "Assets/Models/InvertedCube.obj";
	Mesh* m_pSkyMesh;

	ID3D11DepthStencilState* m_pSkyboxDepth;
	ID3D11DepthStencilState* m_pDefaultDepth;
	Shader* m_pShader;

public:
	Skybox(std::string skyboxLoc);
	~Skybox();

	void UpdateGfx();
};

