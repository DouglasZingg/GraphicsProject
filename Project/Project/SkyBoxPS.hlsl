textureCUBE SkyMap : register(t0);
SamplerState samLinear : register(s0);

struct SKYMAP_VS_OUTPUT   
{
    float4 xyzw : SV_Position; 
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
    float3 worldpos : WorldPos;
};

float4 main(SKYMAP_VS_OUTPUT input) : SV_TARGET
{
    return SkyMap.Sample(samLinear, input.uvw);
}