cbuffer ConstantBuffer : register(b0)
{
	// ワールド変換行列
	matrix World : packoffset(c0);
	// ビュー変換行列
	matrix View : packoffset(c4);
	// 透視射影変換行列
	matrix Projection : packoffset(c8);
	// WVP行列
	matrix WorldViewProjection : packoffset(c12);

	// カメラの位置座標
	float4 ViewPosition : packoffset(c16);
	// ライトの位置座標(平行光源 w = 0, 点光源 w = 1)
	float4 LightPosition : packoffset(c17);
	// マテリアルの表面カラー
	float4 MaterialDiffuse : packoffset(c18);

	// マテリアルの鏡面反射カラー
	float3 MaterialSpecularColor : packoffset(c19);
	// マテリアルの鏡面反射の強さ
	float MaterialSpecularPower : packoffset(c19.w);
}

// 頂点シェーダーへの入力
struct VSInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// 頂点シェーダーからの出力
struct VSOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 worldNormal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// ジオメトリーシェーダーへの入力
typedef VSOutput GSInput;

// ジオメトリーシェーダーからの出力
struct GSOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 worldNormal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// ピクセルシェーダーへの入力
typedef GSOutput PSInput;
