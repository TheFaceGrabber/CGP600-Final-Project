cbuffer Properties : register(b2)
{
    float4 DiffuseColour = { 1, 1, 1, 1 };
    float Smoothness = 1;
    float3 Padding;
};

Texture2D DiffuseTex : register(t1);
Texture2D SpecularTex : register(t2);
Texture2D NormalTex : register(t3);

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
	float4 worldPos:TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BITANGENT;
    float2 uv : TEXCOORD;
	float4 lightSpacePos : POSITION2;
};

#include "ReqInc.hlsl"

VOut vert(VIn v)
{
	VOut output;

    output.position = LocalToWVP(float4(v.position, 1.0f));
    output.uv = v.uv;
    output.normal = v.normal;
    output.tangent = v.tangent;
    output.binormal = cross(output.tangent, output.normal);

	float4 worldPos = LocalToWorld(float4(v.position, 1.0f));
	output.worldPos = worldPos;
    float4 lightSpacePos = mul(DirLightWVP, float4(v.position, 1.0f));
	output.lightSpacePos = lightSpacePos;

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

	float4 worldPos = i.worldPos;

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

	float2 shadowTexCoords;
    shadowTexCoords.x = i.lightSpacePos.x / i.lightSpacePos.w / 2.0f + 0.5f;
    shadowTexCoords.y = -i.lightSpacePos.y / i.lightSpacePos.w / 2.0f + 0.5f;
    
    float pixelDepth = i.lightSpacePos.z/i.lightSpacePos.w;
        
	float lighting = 0;

	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
		(saturate(shadowTexCoords.y) == shadowTexCoords.y))
	{
        float bias = 0.0001 * tan(acos(ndl));
		// Clamp epsilon to a fixed range so it doesn't go overboard.
        bias = clamp(bias, 0, 0.01);
        
        float2 poissonDisk[4];
        poissonDisk[0] = float2(-0.94201624, -0.39906216);
        poissonDisk[1] = float2(0.94558609, -0.76890725);
        poissonDisk[2] = float2(-0.094184101, -0.92938870);
        poissonDisk[3] = float2(0.34495938, 0.29387760);
        
        int interations = 4;

        for (int i = 0; i < interations; i++)
        {
            float d = ShadowMapTex.SampleCmpLevelZero(ShadowsSampler, shadowTexCoords.xy + poissonDisk[i] / 700, pixelDepth - bias).x;
            lighting += d;
        }
        lighting /= interations;
        
        //if (lighting < pixelDepth - bias)
        //    ndl = 0;
        ndl *= lighting;

    }

    float smoothness = specTex.a * Smoothness;
    float3 viewDir = normalize(WorldCameraPosition - worldPos.xyz);
	float gloss = 0;
	if(ndl > 0)
		gloss = BlinnPhong(smoothness, normal, lightDir, viewDir) * ndl;
    float4 specCol = float4(specTex.rgb, 1);

    //From Eric Lengyel "Mathematics for 3D Game Programming and Computer Graphics"
    float4 finalDiff = DiffuseColour * diffTex * AmbientColour + DiffuseColour * diffTex * LightColour * LightStrength * ndl;
    float4 finalSpec = saturate(specCol * LightColour * LightStrength * gloss);
    
    finalCol = finalSpec + finalDiff;
    
    return finalCol;

}
