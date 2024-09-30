Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct PS_IN
{
    float4 xyzw : SV_Position; //system value
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
    float3 worldpos : WorldPos;
};

cbuffer Emissive : register(b0)
{
    float4 dir;
    float4 ambient;
    float4 Ediffuse;
};

float4 main(PS_IN input) : SV_Target
{
    input.nrm = normalize(input.nrm);
    float4 vEmissiveSample = txDiffuse.Sample(samLinear, input.uvw.xy);
    float4 vLitColor = { 1, 1, 0, 1 };
    
    float4 vFinalColor = vEmissiveSample * vLitColor;
    return vFinalColor;
}