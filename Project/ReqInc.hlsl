cbuffer CBuffer0 : register(b0)
{
    float3 LightVector;
    float LightStrength;
    float4 LightColour;
    float4 AmbientColour;
    float3 WorldCameraPosition;
    float padding;
};

cbuffer CBuffer1 : register(b1)
{
    matrix WVP;
    matrix World;

    matrix DirLightWVP;
};

sampler Sampler : register(s0);
SamplerComparisonState  ShadowsSampler : register(s1);

Texture2D ShadowMapTex : register(t0);

float3 FixNormal(float4 packednormal)
{
    return packednormal * 2 - 1;
}

float4 LocalToWorld(float4 localPosition)
{
    return mul(World, localPosition);
}

float3 LocalToWorldNormal(float3 localNormal)
{
    return mul(World, localNormal);
}

float4 LocalToWVP(float4 localPosition)
{
    return mul(WVP, localPosition);

}
