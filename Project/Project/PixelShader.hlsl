Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct OutputVertex
{
    float4 xyzw : SV_Position; //system value
    float3 uv : OCOLOR;
};

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    return shaderTexture.Sample(SampleType, inputPixel.uv.xy);
}