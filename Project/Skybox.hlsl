#include "ReqInc.hlsl"
cbuffer Properties : register(b2)
{
    float SunSize;
    float SunStrength;
    float2 Padding;
};

TextureCube SkyTex : register(t0);
sampler Sampler : register(s0);

struct VIn
{
	float4 position : POSITION;
	float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    //float3 tangent : TANGENT0;
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

float4 frag(VOut i) : SV_TARGET
{
    float4 finalCol;

	float3 v = normalize(i.uv);

	float3 lightDir = -normalize(LightVector);

	finalCol = SkyTex.Sample(Sampler, i.uv);

    float4 sun = min(pow(max(0, dot(v, lightDir)), SunSize), 1) * SunStrength * LightColour;

	return sun+finalCol;
}
