struct VIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 scrnPosition : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BITANGENT;
    float2 uv : TEXCOORD;
};

#include "ReqInc.hlsl"

VOut vert(VIn v)
{
	VOut output;

    output.position = LocalToWVP(float4(v.position, 1.0f));
    output.scrnPosition = float4(v.position, 1.0f);
    output.uv = v.uv;
    output.normal = v.normal;
    output.tangent = v.tangent;
    output.binormal = cross(output.tangent, output.normal);

	return output;
}

float4 frag(VOut i) : SV_TARGET
{
   return float4(1,1,1,1);
}
