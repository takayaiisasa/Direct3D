//=============================================================================
// InputLayout.cpp
// 
//=============================================================================
#include "Game.h"
#include <new>
using namespace std;

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
InputLayout* InputLayout::Create(
	ID3D11Device* device,
	D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
	const void* shaderBytecodeWithInputSignature, SIZE_T bytecodeLength)
{
	// ���̓��C�A�E�g���쐬
	ID3D11InputLayout* inputLayout = nullptr;
	auto hr = device->CreateInputLayout(
		inputElementDescs, numElements,
		shaderBytecodeWithInputSignature, bytecodeLength,
		&inputLayout);

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) InputLayout();
	if (result == nullptr) {
		OutputDebugString(L"VertexBuffer���쐬����ۂɃ������[���s�����܂����B");
		return nullptr;
	}
	result->inputLayout = inputLayout;

	return result;
}

// ���\�[�X��������܂��B
void InputLayout::Release() {
	SAFE_RELEASE(inputLayout);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11InputLayout* InputLayout::GetNativePointer() {
	return inputLayout;
}
