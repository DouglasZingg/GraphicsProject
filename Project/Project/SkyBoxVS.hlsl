#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 xyz : POSITION;
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
};

struct SKYMAP_VS_OUTPUT  
{
    float4 xyzw : SV_Position; 
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
    float3 worldpos : WorldPos;
};
    
cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

SKYMAP_VS_OUTPUT main(InputVertex input)
{
    SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT) 0;
    output.xyzw = float4(input.xyz, 1.0f);
    
    output.xyzw = mul(output.xyzw, worldMatrix);
    output.xyzw = mul(output.xyzw, viewMatrix);
    output.xyzw = mul(output.xyzw, projectionMatrix);
    
    output.worldpos = mul(input.xyz, (float3x3) worldMatrix);
    
    output.uvw = input.xyz;
    
    return output; 
}