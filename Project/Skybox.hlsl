#include "ReqInc.hlsl"
TextureCube SkyTex : register(t0);
sampler Sampler : register(s0);

struct VIn
{
	float4 position : POSITION;
	float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VOut
{
    float4 position : SV_POSITION;
    float3 uv : TEXCOORD;
	float3 normal : NORMAL;
};

VOut vert(VIn v)
{
	VOut output;

	output.normal = LocalToWorldNormal(v.normal);
	output.uv = v.position.xyz;
    output.position = LocalToWVP(v.position);
    
	return output;
}

float Sun(float3 sunDir, float3 skyPos)
{
	float3 diff = sunDir - skyPos;
	float dist = length(diff);
	float spot = 1 - smoothstep(0, 1, dist);
	return 1 - pow(0.125, spot);
}

float4 frag(VOut i) : SV_TARGET
{
    float4 finalCol;

	float3 v = normalize(i.uv);

	float3 lightDir = -normalize(LightVector);

	finalCol = SkyTex.Sample(Sampler, i.uv);

	float4 sun = min(pow(max(0, dot(v, lightDir)), 500), 1) * 2 * LightColour;

	return sun + finalCol;
}
