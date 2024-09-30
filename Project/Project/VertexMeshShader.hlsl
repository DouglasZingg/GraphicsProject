//rule of three:
//Three things must match (DATA)
//1. C++ Vertex Stuct
//2. Inout Layout
//#. HLSL Vertex Struct

#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 xyz : POSITION;
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
};

struct OutputVertex
{
    float4 xyzw : SV_Position; //system value
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

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex) 0;
    output.xyzw = float4(input.xyz, 1);
    
    //Do math here
    output.xyzw = mul(output.xyzw, worldMatrix);
    output.worldpos = mul(input.xyz, (float3x3) worldMatrix);
    output.xyzw = mul(output.xyzw, viewMatrix);
    output.xyzw = mul(output.xyzw, projectionMatrix);
    
    output.uvw = input.uvw;
    output.nrm = mul(input.nrm, (float3x3) worldMatrix);
    
    return output;
}