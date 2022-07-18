cbuffer ConstantBuffer : register(b0)
{
	// ���[���h�ϊ��s��
	matrix World : packoffset(c0);
	// �r���[�ϊ��s��
	matrix View : packoffset(c4);
	// �����ˉe�ϊ��s��
	matrix Projection : packoffset(c8);
	// WVP�s��
	matrix WorldViewProjection : packoffset(c12);

	// �J�����̈ʒu���W
	float4 ViewPosition : packoffset(c16);
	// ���C�g�̈ʒu���W(���s���� w = 0, �_���� w = 1)
	float4 LightPosition : packoffset(c17);
	// �}�e���A���̕\�ʃJ���[
	float4 MaterialDiffuse : packoffset(c18);

	// �}�e���A���̋��ʔ��˃J���[
	float3 MaterialSpecularColor : packoffset(c19);
	// �}�e���A���̋��ʔ��˂̋���
	float MaterialSpecularPower : packoffset(c19.w);
}

// ���_�V�F�[�_�[�ւ̓���
struct VSInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// ���_�V�F�[�_�[����̏o��
struct VSOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 worldNormal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// �W�I���g���[�V�F�[�_�[�ւ̓���
typedef VSOutput GSInput;

// �W�I���g���[�V�F�[�_�[����̏o��
struct GSOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 worldNormal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// �s�N�Z���V�F�[�_�[�ւ̓���
typedef GSOutput PSInput;
