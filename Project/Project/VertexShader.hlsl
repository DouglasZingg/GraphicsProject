//rule of three:
//Three things must match (DATA)
//1. C++ Vertex Stuct
//2. Inout Layout
//#. HLSL Vertex Struct

#pragma pack_matrix(row_major)

struct InputVertex
{
    float4 xyzw : POSITION;
    float3 uv : COLOR;
};

struct OutputVertex
{
    float4 xyzw : SV_Position; //system value
    float3 uv : OCOLOR;
};

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex) 0;
    output.uv = input.uv;
    output.xyzw = input.xyzw;
    
    return output;
}