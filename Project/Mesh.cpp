#include "Mesh.h"
#include "Direct3D.h"
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
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
		throw "Model file could not be opened. Reason: Location does not exists";
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
				p0 = uint32_t(ptrdiff_t(positions.size()) + n0);
			else
				p0 -= 1;
			if (p1 < 0)
				p1 = uint32_t(ptrdiff_t(positions.size()) + n1);
			else
				p1 -= 1;
			if (p2 < 0)
				p2 = uint32_t(ptrdiff_t(positions.size()) + n2);
			else
				p2 -= 1;

			if (n0 <= 0)
				n0 = uint32_t(ptrdiff_t(normals.size()) + n0);
			else
				n0 -= 1;
			if (n1 <= 0)
				n1 = uint32_t(ptrdiff_t(normals.size()) + n1);
			else
				n1 -= 1;
			if (n2 <= 0)
				n2 = uint32_t(ptrdiff_t(normals.size()) + n2);
			else
				n2 -= 1;

			posIndex.push_back(p0);
			posIndex.push_back(p1);
			posIndex.push_back(p2);
			nrmIndex.push_back(n0);
			nrmIndex.push_back(n1);
			nrmIndex.push_back(n2);
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

	XMVECTOR*tan1 = new XMVECTOR[verts.size() * 2];
	ZeroMemory(tan1, verts.size() * 2 * sizeof(XMVECTOR));

	for (int i = 0; i < posIndex.size(); i+=3)
	{
		int i1 = posIndex[i];
		int i2 = posIndex[i + 1];
		int i3 = posIndex[i + 2];

		int v1 = posIndex[i1];
		int v2 = posIndex[i2];
		int v3 = posIndex[i3];
		int u1 = uvIndex[i1];
		int u2 = uvIndex[i2];
		int u3 = uvIndex[i3];
		XMFLOAT3 p1 = positions[v1];
		XMFLOAT3 p2 = positions[v2];
		XMFLOAT3 p3 = positions[v3];

		XMFLOAT2 w1 = uvs[u1];
		XMFLOAT2 w2 = uvs[u2];
		XMFLOAT2 w3 = uvs[u3];

		float x1 = p2.x - p1.x;
		float x2 = p3.x - p1.x;
		float y1 = p2.y - p1.y;
		float y2 = p3.y - p1.y;
		float z1 = p2.z - p1.z;
		float z2 = p3.z - p1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 0;

		float div = (s1 * t2 - s2 * t1);
		if(div != 0)
			r = 1.0F / div;

		XMVECTOR sdir = XMLoadFloat3(&XMFLOAT3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r));
		//XMVECTOR tdir = XMLoadFloat3(&XMFLOAT3((sx1 * x2 - sx2 * x1) * r, (sx1 * y2 - sx2 * y1) * r,
		//	(sx1 * z2 - sx2 * z1) * r));

		tan1[i] = XMVectorAdd(tan1[i],sdir);
		tan1[i + 1] = XMVectorAdd(tan1[i+1], sdir);
		tan1[i + 2] = XMVectorAdd(tan1[i+2], sdir);
	}

	for (int i = 0; i < verts.size(); i++)
	{
		XMVECTOR n = XMLoadFloat3(&verts[i].Norm);
		XMVECTOR t = tan1[i];
		float dot = XMVectorGetX(XMVector3Dot(n, t));
		XMVECTOR tan = XMVectorSubtract(t, XMVectorScale(n, dot));
		tan = XMVector3Normalize(tan);
		XMFLOAT3 finalTan;
		XMStoreFloat3(&finalTan, tan);
		verts[i].Tan = finalTan;
	}

	ApplyVertices(verts, true);

	delete[] tan1;
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

void Mesh::SetShader(Shader* shader)
{
	if (shader)
		m_pShader = shader;
}
