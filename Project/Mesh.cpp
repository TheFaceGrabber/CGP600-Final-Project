#include "Mesh.h"
#include "Direct3D.h"
#include <fstream>
#include <strstream>

Mesh::~Mesh()
{
	if (g_pVertexBuffer) 
		g_pVertexBuffer->Release();

	m_vertices.clear();

	delete m_pShader;
}

void Mesh::LoadFromFile(std::string loc)
{
	vector<DefaultVertex> verts;
	vector<XMFLOAT3> positions;
	vector<XMFLOAT2> uvs;
	vector<XMFLOAT3> normals;
	vector<int> nrmIndex;
	vector<int> posIndex;
	vector<int> uvIndex;

	std::ifstream f(loc);
	if (!f.is_open()) 
	{
		throw "Model file could not be openned. Reason: Location does not exists";
		return;
	}

	while (!f.eof())
	{
		const int size = 128;
		char line[size];
		f.getline(line, size);

		strstream s;
		s << line;

		std::string type;
		s >> type;
		if (type == "v")
		{
			XMFLOAT3 v;
			s >> v.x >> v.y >> v.z;
			positions.push_back(v);
		}
		else if(type == "vt")
		{
			XMFLOAT2 uv;
			s >> uv.x >> uv.y;
			uvs.push_back(uv);
		}
		else if (type == "vn")
		{
			XMFLOAT3 nrm;
			s >> nrm.x >> nrm.y >> nrm.z;
			normals.push_back(nrm);
		}
		else if (type == "f")
		{
			int strokeCount = 0;
			bool NormalsOnly = false;

			for (int i = 0; i < size; i++)
			{
				if (line[i] == '/') {
					strokeCount++;
					if (line[i - 1] == '/') 
						NormalsOnly = true;
				}
			}

			char slash;
			short p0 = 0, p1 = 0, p2 = 0;
			short t0 = 0, t1 = 0, t2 = 0;
			short n0 = 0, n1 = 0, n2 = 0;
			if (strokeCount == 0) //Just vertex points
			{
				s >> p0 >> p1 >> p2;
			}
			else if (strokeCount == 3) //Point and tex coord
			{
				s >>
					p0 >> slash >> t0 >>
					p1 >> slash >> t1 >>
					p2 >> slash >> t2;
			}
			else if (strokeCount >= 6)
			{
				if (NormalsOnly) //Point and normals
				{
					s >>
						p0 >> slash >> slash >> n0 >>
						p1 >> slash >> slash >> n1 >>
						p2 >> slash >> slash >> n2;
				}
				else //Point normals and tex coord
				{
					s >>
						p0 >> slash >> t0 >> slash >> n0 >>
						p1 >> slash >> t1 >> slash >> n1 >>
						p2 >> slash >> t2 >> slash >> n2;
				}
				//}
			}

			if (p0 < 0)
				p0 = uint32_t(ptrdiff_t(positions.size()) + p0);
			else
				p0 -= 1;
			if (p1 < 0)
				p1 = uint32_t(ptrdiff_t(positions.size()) + p1);
			else
				p1 -= 1;
			if (p2 < 0)
				p2 = uint32_t(ptrdiff_t(positions.size()) + p2);
			else
				p2 -= 1;

			posIndex.push_back(p0);
			posIndex.push_back(p1);
			posIndex.push_back(p2);
			nrmIndex.push_back(n0 - 1);
			nrmIndex.push_back(n1 - 1);
			nrmIndex.push_back(n2 - 1);
			uvIndex.push_back(t0 - 1);
			uvIndex.push_back(t1 - 1);
			uvIndex.push_back(t2 - 1);
		}
	}

	for (int i = 0; i < posIndex.size(); i++)
	{
		DefaultVertex vert;
		int v = posIndex[i];
		vert.Pos = positions[v];
		
		if(!nrmIndex.empty())
		{
			int n = nrmIndex[i];
			vert.Norm = normals[n];
		}

		if(!uvIndex.empty())
		{
			int u = uvIndex[i];
			vert.UV = uvs[u];
		}

		verts.push_back(vert);
	}

	ApplyVertices(verts, true);
}

void Mesh::ApplyVertices(vector<DefaultVertex> verts, bool applyMesh)
{
	m_vertices = verts;

	if (applyMesh)
		ApplyMesh();
}

void Mesh::Draw()
{
	GetShader()->Update();
	Direct3D::GetInstance()->Draw(this);
}

void Mesh::UpdateMaterial(string materialLoc)
{
	m_pShader = Shader::LoadFromMaterial(materialLoc);
}

void Mesh::ApplyMesh()
{
	//Since we have updated the mesh, we need to clear the previous vertex buffer and create a new one
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										
	bufferDesc.ByteWidth = sizeof(DefaultVertex) * m_vertices.capacity();										
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							
	Direct3D::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);		

	DefaultVertex* verts = m_vertices.data();

	D3D11_MAPPED_SUBRESOURCE ms;

	Direct3D::GetInstance()->GetContext()->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	long vertSize = sizeof(DefaultVertex);

	memcpy(ms.pData, verts, vertSize * m_vertices.capacity());

	Direct3D::GetInstance()->GetContext()->Unmap(g_pVertexBuffer, NULL);

	UpdateMaterial("Assets/Materials/Tiles.jmtl");
	/*m_pShader = new Shader("BlinnPhong.hlsl");
	m_pShader->ApplyTexture("Assets/Textures/hardwood-brown-planks-albedo.png", 0);
	m_pShader->ApplyTexture("Assets/Textures/hardwood-brown-planks-specular.tif", 1);
	*/
}
