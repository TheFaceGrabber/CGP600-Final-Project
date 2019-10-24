#pragma once
#include "Component.h"
#include "Mesh.h"

class MeshRenderer :
	public Component
{
private:
	Mesh* m_pMesh;
public:
	~MeshRenderer();

	void SetMesh(Mesh* mesh) { m_pMesh = mesh; }
	Mesh* GetMesh() { return m_pMesh; }

	void UpdateGfx() override;
};

