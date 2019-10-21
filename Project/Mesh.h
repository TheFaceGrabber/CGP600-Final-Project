#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <vector>
#include <xnamath.h>
#include <string>
#include "Shader.h"

using namespace std;

struct DefaultVertex
{
	XMFLOAT3	Pos;
	XMFLOAT3	Norm;
	XMFLOAT2	UV;
};

class Mesh
{
private:
	ID3D11Buffer* g_pVertexBuffer;

	Shader* m_pShader;

	vector<DefaultVertex> m_vertices;

public:
	~Mesh();

	ID3D11Buffer* GetVertexBuffer() { return g_pVertexBuffer; }
	Shader* GetShader() { return m_pShader; }
	vector<DefaultVertex> GetVertices() { return m_vertices; }

	void LoadFromFile(std::string loc);

	void ApplyVertices(vector<DefaultVertex> verts, bool applyMesh = false);

	void Draw();
	void ApplyMesh();
};