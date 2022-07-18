#include "BasicShader.hlsli"

Texture2D diffuseTexture;
SamplerState diffuseTextureSampler;

float4 main(PSInput input) : SV_TARGET
{
	float4 worldPosition = input.worldPosition;
	// 面から光源を指す正規化ベクトルL(平行光源(w=0)と点光源(w=1))
	float3 light = normalize(LightPosition.xyz - LightPosition.w * worldPosition.xyz);

	// ワールド空間上の法線ベクトルN
	float3 worldNormal = input.worldNormal;
	// 拡散反射
	float diffuse = max(dot(light, worldNormal), 0);
	float3 diffuseColor = diffuse * MaterialDiffuse.xyz;

	// 鏡面反射
	float3 reflect = 2 * input.worldNormal * dot(input.worldNormal, light) - light;
	float3 viewDir = normalize(ViewPosition - input.worldPosition).xyz;
	float specular = pow(saturate(dot(reflect, viewDir)), MaterialSpecularPower);
	float3 specularColor = specular * MaterialSpecularColor.xyz;

	float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.texCoord);

	return float4(texel.xyz * diffuseColor + specularColor, MaterialDiffuse.w * texel.w);
}