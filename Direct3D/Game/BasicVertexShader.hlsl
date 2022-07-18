#include "BasicShader.hlsli"

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(input.position, WorldViewProjection);
	output.worldPosition = mul(input.position, World);
	output.worldNormal = normalize(mul(input.normal, (float3x3)World));
	output.texCoord = input.texCoord;
	return output;
}
