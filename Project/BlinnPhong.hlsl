cbuffer Properties : register(b2)
{
    float4 DiffuseColour = { 1, 1, 1, 1 };
    float Smoothness = 1;
    float3 Padding;
};

Texture2D DiffuseTex : register(t0);
Texture2D SpecularTex : register(t1);
Texture2D NormalTex : register(t2);
sampler Sampler : register(s0);

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

float BlinnPhong(float smoothness, float3 normal, float3 lightDir, float3 viewDir)
{
    if (smoothness > 0.99f)
        smoothness = 0.99f;

    //Smoothness to exponnent found here
    //http://mfactorgames.com/2017/11/22/conversion-between-specular-exponent-specular-roughness-and-specular-glossiness/
    float shininess = 2 / pow((1 - smoothness), 2) - 2;

    //From Eric Lengyel "Mathematics for 3D Game Programming and Computer Graphics"
    float3 halfWay = normalize(lightDir + viewDir);
    float angle = max(dot(halfWay, normal), 0.0);
    return pow(angle, shininess);
}

float4 frag(VOut i) : SV_TARGET
{
    float4 finalCol;

    float4 worldPos = LocalToWorld(i.scrnPosition);

    i.normal = normalize(LocalToWorldNormal(i.normal));
    i.tangent = normalize(LocalToWorldNormal(i.tangent));
    i.binormal = normalize(LocalToWorldNormal(i.binormal));

    float3 normal = FixNormal(NormalTex.Sample(Sampler, i.uv));
    float3x3 normMat = float3x3(i.tangent, i.binormal, i.normal);
    normal = normalize(mul(normal, normMat));
    float3 lightDir = -normalize(LightVector);

    float4 diffTex = DiffuseTex.Sample(Sampler, i.uv);
    float4 specTex = SpecularTex.Sample(Sampler, i.uv);

    float ndl = saturate(dot(normal, lightDir));
    
    float smoothness = specTex.a * Smoothness;
    float3 viewDir = normalize(WorldCameraPosition - worldPos.xyz);
    float gloss = BlinnPhong(smoothness, normal, lightDir, viewDir) * ndl;
    float4 specCol = float4(specTex.rgb, 1);

    //From Eric Lengyel "Mathematics for 3D Game Programming and Computer Graphics"
    float4 finalDiff = DiffuseColour * diffTex * AmbientColour + DiffuseColour * diffTex * LightColour * LightStrength * ndl;
    float4 finalSpec = saturate(specCol * LightColour * LightStrength * gloss);
    
    finalCol = finalSpec + finalDiff;
    
    return finalCol;//float4(finalCol, 1);

}
