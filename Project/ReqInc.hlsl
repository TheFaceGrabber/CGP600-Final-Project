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
};

//Refernce specular for non-metals
//0.22  0 - 1
//51    0 -255

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
