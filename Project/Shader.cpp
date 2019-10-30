#include "Shader.h"
#include "Direct3D.h"
#include "ConstantBuffers.h"
#include <fstream>
#include <strstream>

Shader::Shader(std::string shaderFilePath)
{
#pragma region Init Shaders
	ID3DBlob* VS, * PS, * error;

	D3DX11CompileFromFile(shaderFilePath.c_str(), 0, 0, VSFunc.c_str(), "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
	}

	D3DX11CompileFromFile(shaderFilePath.c_str(), 0, 0, PSFunc.c_str(), "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
	}

	//Create shader objects
	Direct3D::GetInstance()->GetDevice()->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);

	Direct3D::GetInstance()->GetDevice()->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	Direct3D::GetInstance()->GetDevice()->CreateInputLayout(iedesc, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);
#pragma endregion 
#pragma region Init Default Sampler
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	
	Direct3D::GetInstance()->GetDevice()->CreateSamplerState(&sampler_desc, &g_pDefaultSampler);

	Direct3D::GetInstance()->GetContext()->PSSetSamplers(0, 1, &g_pDefaultSampler);
#pragma endregion 
}

Shader::~Shader()
{
	if (g_pInputLayout)g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDefaultSampler)g_pDefaultSampler->Release();

	for (int i = 0; i < g_pTextures.size(); i++)
	{
		if (g_pTextures[i])
			g_pTextures[i]->Release();
	}

	g_pTextures.clear();
	m_matProperties.clear();
	delete m_Data;
}

void Shader::ApplyTexture(std::string textureLocation, int index)
{
	if(g_pTextures.size() <= index)
	{
		g_pTextures.resize(index + 1);
	}

	D3DX11CreateShaderResourceViewFromFile(Direct3D::GetInstance()->GetDevice(), textureLocation.c_str(), NULL, NULL, &g_pTextures[index], NULL);
	Direct3D::GetInstance()->GetContext()->PSSetShaderResources(index, 1, &g_pTextures[index]);
}

void* Shader::GetData()
{
	return m_Data;
}

void Shader::SetData(void* data)
{
	m_Data = data;

	if (m_Data == nullptr)
		return;

	ConstantBuffers::GetInstance()->Bind(BUFFER_SHADER_PROPERTIES, data);
}

void Shader::Update()
{
	void* data = m_matProperties.data();

	SetData(data);

	for (int i = 0; i < g_pTextures.size(); i++)
	{
		Direct3D::GetInstance()->GetContext()->PSSetShaderResources(i, 1, &g_pTextures[i]);
	}
}

Shader* Shader::LoadFromMaterial(std::string materialFile)
{
	std::ifstream f(materialFile);
	if (!f.is_open()) return nullptr;
	
	vector<float> properties;
	vector<string> textures;
	string shaderType;

	while (!f.eof())
	{
		const int size = 128;
		char line[size];
		f.getline(line, size);

		strstream s;
		s << line;
		std::string type;
		s >> type;

		if(type == "shader")
		{
			s >> shaderType;
		}
		else if(type == "property")
		{
			float t;
			s >> t;
			properties.push_back(t);
		}
		else if(type == "texture")
		{
			string tex;
			s >> tex;
			textures.push_back(tex);
		}
	}

	Shader* shader = new Shader(shaderType + ".hlsl");
	shader->m_matProperties = properties;

	for (int i = 0; i < textures.size(); i++)
	{
		shader->ApplyTexture(textures[i], i);
	}

	return shader;
}
