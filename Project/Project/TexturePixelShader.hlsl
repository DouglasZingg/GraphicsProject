Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer DIRECTIONAL : register(b0)
{
    float4 directionallightDir;
    float4 directionallightColor;
};

cbuffer Point : register(b1)
{
    float4 pPos;
    float4 pRange;
    float4 pAtt;
    float4 pAmbient;
    float4 pDiffuse;
};

cbuffer Spotlight : register(b2)
{
    float4 sPos;
    float4 sRange;
    float4 sDir;
    float4 sCone;
    float4 sAtt;
    float4 sAmbient;
    float4 sDiffuse;
};

struct PS_IN
{
    float4 xyzw : SV_Position; //system value
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
    float3 worldpos : WorldPos;
};

float3 pointlight(PS_IN input)
{
    //Point
    input.nrm = normalize(input.nrm);
    float4 pdiffuse = txDiffuse.Sample(samLinear, input.uvw.xy);
    float3 pointfinal = float3(0.0f, 0.0f, 0.0f);
    float3 lightToPixelVec = pPos.xyz - input.worldpos;
    float d = length(lightToPixelVec);
    float3 finalAmbient = pdiffuse * pAmbient;
    if (d > pRange.x)
        return float4(finalAmbient, pdiffuse.a);
    lightToPixelVec /= d;
    float howMuchLight = dot(lightToPixelVec, input.nrm);
    if (howMuchLight > 0.0f)
    {
        pointfinal += howMuchLight * pdiffuse.xyz * pDiffuse.xyz;
        pointfinal /= pAtt[0] + (pAtt[1] * d) + (pAtt[2] * (d * d));
    }
    pointfinal = saturate(pointfinal + finalAmbient);
    return pointfinal;
}

float3 spotlight(PS_IN input)
{
    input.nrm = normalize(input.nrm);
    float4 diffuse = txDiffuse.Sample(samLinear, input.uvw.xy);
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    float3 lightToPixelVec = sPos.xyz - input.worldpos;
    float d = length(lightToPixelVec);
    float3 finalAmbient = diffuse * sAmbient;
    if (d > sRange.x)
        return float4(finalAmbient, diffuse.a);
    lightToPixelVec /= d;
    float howMuchLight = dot(lightToPixelVec, input.nrm);
    if (howMuchLight > 0.0f)
    {
        finalColor += diffuse.xyz * sDiffuse.xyz;
        finalColor /= (sAtt[0] + (sAtt[1] * d)) + (sAtt[2] * (d * d));
        finalColor *= pow(max(dot(-lightToPixelVec, sDir.xyz), 0.0f), sCone.x);
    }
    finalColor = saturate(finalColor + finalAmbient);
    return finalColor;
}


float4 main(PS_IN input) : SV_TARGET
{
    //directional
    float directionalratio = dot(-normalize(directionallightDir.xyz), input.nrm);
    directionalratio = saturate(directionalratio);
    float3 directionalfinal = directionalratio * directionallightColor * txDiffuse.Sample(samLinear, input.uvw.xy);
    
    //point
    float3 pointfinal = pointlight(input);
    
    //spot
    float3 spotfinal = spotlight(input);
    
    float3 final = saturate(directionalfinal + pointfinal + spotfinal);
    //return float4(final, 1.0f);
    return txDiffuse.Sample(samLinear, input.uvw.xy);
}