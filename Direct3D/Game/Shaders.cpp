//=============================================================================
// Shaders.cpp
// 
//=============================================================================
#include "Game.h"
#include "BasicVertexShader.h"	// �V�F�[�_�[���R���p�C�������w�b�_�[�t�@�C��
#include "BasicPixelShader.h"
#include "BasicGeometryShader.h"
#include <new>
using namespace std;

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
BasicVertexShader* BasicVertexShader::Create(ID3D11Device* graphicsDevice)
{
	ID3D11VertexShader* vertexShader = nullptr;
	// ���_�V�F�[�_�[�̍쐬
	auto hr = graphicsDevice->CreateVertexShader(
		g_BasicVertexShader,
		ARRAYSIZE(g_BasicVertexShader),
		NULL,
		&vertexShader);
	if (FAILED(hr)) {
		return nullptr;
	}

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) BasicVertexShader();
	if (result == nullptr) {
		return nullptr;
	}
	result->shader = vertexShader;
	return result;
}

// ���\�[�X��������܂��B
void BasicVertexShader::Release() {
	SAFE_RELEASE(shader);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11VertexShader* BasicVertexShader::GetNativePointer() {
	return shader;
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
BasicGeometryShader* BasicGeometryShader::Create(ID3D11Device* graphicsDevice)
{
	ID3D11GeometryShader* shader = nullptr;
	// �V�F�[�_�[�̍쐬
	auto hr = graphicsDevice->CreateGeometryShader(
		g_BasicGeometryShader,
		ARRAYSIZE(g_BasicGeometryShader),
		NULL,
		&shader);
	if (FAILED(hr)) {
		return nullptr;
	}

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) BasicGeometryShader();
	if (result == nullptr) {
		return nullptr;
	}
	result->shader = shader;
	return result;
}

// ���\�[�X��������܂��B
void BasicGeometryShader::Release() {
	SAFE_RELEASE(shader);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11GeometryShader* BasicGeometryShader::GetNativePointer() {
	return shader;
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
BasicPixelShader* BasicPixelShader::Create(ID3D11Device* graphicsDevice)
{
	ID3D11PixelShader* shader = nullptr;
	// �V�F�[�_�[�̍쐬
	auto hr = graphicsDevice->CreatePixelShader(
		g_BasicPixelShader,
		ARRAYSIZE(g_BasicPixelShader),
		NULL,
		&shader);
	if (FAILED(hr)) {
		return nullptr;
	}

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) BasicPixelShader();
	if (result == nullptr) {
		return nullptr;
	}
	result->shader = shader;
	return result;
}

// ���\�[�X��������܂��B
void BasicPixelShader::Release() {
	SAFE_RELEASE(shader);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11PixelShader* BasicPixelShader::GetNativePointer() {
	return shader;
}
