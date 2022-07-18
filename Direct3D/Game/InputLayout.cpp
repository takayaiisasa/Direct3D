//=============================================================================
// InputLayout.cpp
// 
//=============================================================================
#include "Game.h"
#include <new>
using namespace std;

// このクラスの新しいインスタンスを作成します。
InputLayout* InputLayout::Create(
	ID3D11Device* device,
	D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
	const void* shaderBytecodeWithInputSignature, SIZE_T bytecodeLength)
{
	// 入力レイアウトを作成
	ID3D11InputLayout* inputLayout = nullptr;
	auto hr = device->CreateInputLayout(
		inputElementDescs, numElements,
		shaderBytecodeWithInputSignature, bytecodeLength,
		&inputLayout);

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) InputLayout();
	if (result == nullptr) {
		OutputDebugString(L"VertexBufferを作成する際にメモリーが不足しました。");
		return nullptr;
	}
	result->inputLayout = inputLayout;

	return result;
}

// リソースを解放します。
void InputLayout::Release() {
	SAFE_RELEASE(inputLayout);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11InputLayout* InputLayout::GetNativePointer() {
	return inputLayout;
}
