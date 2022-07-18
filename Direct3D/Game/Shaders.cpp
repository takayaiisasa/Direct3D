//=============================================================================
// Shaders.cpp
// 
//=============================================================================
#include "Game.h"
#include "BasicVertexShader.h"	// シェーダーをコンパイルしたヘッダーファイル
#include "BasicPixelShader.h"
#include "BasicGeometryShader.h"
#include <new>
using namespace std;

// このクラスの新しいインスタンスを作成します。
BasicVertexShader* BasicVertexShader::Create(ID3D11Device* graphicsDevice)
{
	ID3D11VertexShader* vertexShader = nullptr;
	// 頂点シェーダーの作成
	auto hr = graphicsDevice->CreateVertexShader(
		g_BasicVertexShader,
		ARRAYSIZE(g_BasicVertexShader),
		NULL,
		&vertexShader);
	if (FAILED(hr)) {
		return nullptr;
	}

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) BasicVertexShader();
	if (result == nullptr) {
		return nullptr;
	}
	result->shader = vertexShader;
	return result;
}

// リソースを解放します。
void BasicVertexShader::Release() {
	SAFE_RELEASE(shader);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11VertexShader* BasicVertexShader::GetNativePointer() {
	return shader;
}

// このクラスの新しいインスタンスを作成します。
BasicGeometryShader* BasicGeometryShader::Create(ID3D11Device* graphicsDevice)
{
	ID3D11GeometryShader* shader = nullptr;
	// シェーダーの作成
	auto hr = graphicsDevice->CreateGeometryShader(
		g_BasicGeometryShader,
		ARRAYSIZE(g_BasicGeometryShader),
		NULL,
		&shader);
	if (FAILED(hr)) {
		return nullptr;
	}

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) BasicGeometryShader();
	if (result == nullptr) {
		return nullptr;
	}
	result->shader = shader;
	return result;
}

// リソースを解放します。
void BasicGeometryShader::Release() {
	SAFE_RELEASE(shader);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11GeometryShader* BasicGeometryShader::GetNativePointer() {
	return shader;
}

// このクラスの新しいインスタンスを作成します。
BasicPixelShader* BasicPixelShader::Create(ID3D11Device* graphicsDevice)
{
	ID3D11PixelShader* shader = nullptr;
	// シェーダーの作成
	auto hr = graphicsDevice->CreatePixelShader(
		g_BasicPixelShader,
		ARRAYSIZE(g_BasicPixelShader),
		NULL,
		&shader);
	if (FAILED(hr)) {
		return nullptr;
	}

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) BasicPixelShader();
	if (result == nullptr) {
		return nullptr;
	}
	result->shader = shader;
	return result;
}

// リソースを解放します。
void BasicPixelShader::Release() {
	SAFE_RELEASE(shader);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11PixelShader* BasicPixelShader::GetNativePointer() {
	return shader;
}
