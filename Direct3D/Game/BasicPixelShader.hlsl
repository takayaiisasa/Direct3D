#include "BasicShader.hlsli"

Texture2D diffuseTexture;
SamplerState diffuseTextureSampler;

float4 main(PSInput input) : SV_TARGET
{
	float4 worldPosition = input.worldPosition;
	// �ʂ���������w�����K���x�N�g��L(���s����(w=0)�Ɠ_����(w=1))
	float3 light = normalize(LightPosition.xyz - LightPosition.w * worldPosition.xyz);

	// ���[���h��ԏ�̖@���x�N�g��N
	float3 worldNormal = input.worldNormal;
	// �g�U����
	float diffuse = max(dot(light, worldNormal), 0);
	float3 diffuseColor = diffuse * MaterialDiffuse.xyz;

	// ���ʔ���
	float3 reflect = 2 * input.worldNormal * dot(input.worldNormal, light) - light;
	float3 viewDir = normalize(ViewPosition - input.worldPosition).xyz;
	float specular = pow(saturate(dot(reflect, viewDir)), MaterialSpecularPower);
	float3 specularColor = specular * MaterialSpecularColor.xyz;

	float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.texCoord);

	return float4(texel.xyz * diffuseColor + specularColor, MaterialDiffuse.w * texel.w);
}