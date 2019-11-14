#pragma once
#include <d3d11.h>
#include <string>
#include <vector>

class Shader
{
private:
	const std::string PSFunc = "frag";
	const std::string VSFunc = "vert";

	ID3D11SamplerState* g_pDefaultSampler;
	ID3D11SamplerState* g_pShadowsSampler;

	ID3D11VertexShader* g_pVertexShader;
	ID3D11PixelShader* g_pPixelShader;
	ID3D11InputLayout* g_pInputLayout;

	std::vector<ID3D11ShaderResourceView*> g_pTextures;

	void* m_Data;

	std::vector<float> m_matProperties;

public:
	Shader(std::string shaderFilePath);
	~Shader();

	ID3D11VertexShader* GetVertexShader() { return g_pVertexShader; }
	ID3D11PixelShader* GetPixelShader() { return g_pPixelShader; }
	ID3D11InputLayout* GetInputLayout() { return g_pInputLayout; }

	void ApplyTexture(std::string textureLocation, int index);

	void* GetData();
	void SetData(void* data);

	void Update();

	static Shader* LoadFromMaterial(std::string materialFile);
};

