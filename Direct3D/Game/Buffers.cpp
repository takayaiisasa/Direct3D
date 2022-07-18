//=============================================================================
// Buffers.cpp
// 
//=============================================================================
#include "Game.h"
#include <new>
using namespace std;

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
VertexBuffer* VertexBuffer::Create(
	ID3D11Device* graphicsDevice,
	void* vertices, UINT byteWidth)
{
	// ���_�o�b�t�@�[���쐬
	ID3D11Buffer* buffer = nullptr;

	// �쐬���钸�_�o�b�t�@�[�ɂ��Ă̋L�q
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;	// �쐬����o�b�t�@�[�̃T�C�Y(bytes)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;	// �o�b�t�@�[�̎g�p���@�i�Ƃ肠����DEFAULT�j
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// Vertex Buffer�Ƃ��ė��p����
	bufferDesc.CPUAccessFlags = 0;			// CPU����̓ǂݏ����Ɏg��Ȃ��ꍇ��0
	bufferDesc.MiscFlags = 0;				// �I�v�V�����̃t���O
	bufferDesc.StructureByteStride = 0;		// ���_�o�b�t�@�[�Ƃ��Ďg���Ȃ�0
	HRESULT hr;
	if (vertices == nullptr) {
		hr = graphicsDevice->CreateBuffer(&bufferDesc, nullptr, &buffer);
	}
	else {
		// ���������Ɏg�p���錳�f�[�^
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = vertices;
		hr = graphicsDevice->CreateBuffer(&bufferDesc, &initialData, &buffer);
	}
	if (FAILED(hr)) {
		OutputDebugString(L"���_�o�b�t�@�[�̍쐬�Ɏ��s���܂����B");
		return nullptr;
	}

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) VertexBuffer();
	if (result == nullptr) {
		OutputDebugString(L"VertexBuffer���쐬����ۂɃ������[���s�����܂����B");
		return nullptr;
	}
	result->buffer = buffer;
	return result;
}

// ���\�[�X��������܂��B
void VertexBuffer::Release() {
	SAFE_RELEASE(buffer);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11Buffer* VertexBuffer::GetNativePointer() {
	return buffer;
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
IndexBuffer* IndexBuffer::Create(
	ID3D11Device* device, UINT sizeOfBufferInBytes)
{
	return Create(device, sizeOfBufferInBytes, nullptr);
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
IndexBuffer* IndexBuffer::Create(
	ID3D11Device* graphicsDevice,
	UINT byteWidth, void* source)
{
	// �C���f�b�N�X�o�b�t�@�[���쐬
	ID3D11Buffer* buffer = nullptr;

	// �쐬����C���f�b�N�X�o�b�t�@�[�ɂ��Ă̋L�q
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr;
	if (source == nullptr) {
		hr = graphicsDevice->CreateBuffer(&bufferDesc, nullptr, &buffer);
	}
	else {
		// ���������Ɏg�p���錳�f�[�^
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = source;
		hr = graphicsDevice->CreateBuffer(&bufferDesc, &initialData, &buffer);
	}
	if (FAILED(hr)) {
		OutputDebugString(L"���_�o�b�t�@�[�̍쐬�Ɏ��s���܂����B");
		return nullptr;
	}

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) IndexBuffer();
	if (result == nullptr) {
		OutputDebugString(L"VertexBuffer���쐬����ۂɃ������[���s�����܂����B");
		return nullptr;
	}
	result->buffer = buffer;
	result->sizeInBytes = byteWidth;
	return result;
}

// ���\�[�X��������܂��B
void IndexBuffer::Release()
{
	SAFE_RELEASE(buffer);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11Buffer* IndexBuffer::GetNativePointer()
{
	return buffer;
}

// �w�肳�ꂽ�f�[�^���o�b�t�@�[�֏������݂܂��B
void IndexBuffer::SetData(void* data)
{
	ID3D11Device* device = nullptr;
	buffer->GetDevice(&device);
	ID3D11DeviceContext* context = nullptr;
	device->GetImmediateContext(&context);

	context->UpdateSubresource(buffer, 0, NULL, data, 0, 0);

	SAFE_RELEASE(context);
	SAFE_RELEASE(device);
}

// �C���f�b�N�X�o�b�t�@�[�̃T�C�Y(�o�C�g�P��)���擾���܂��B
UINT IndexBuffer::GetSizeInBytes()
{
	return sizeInBytes;
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
ConstantBuffer* ConstantBuffer::Create(
	ID3D11Device* device, UINT sizeOfBufferInBytes)
{
	return Create(device, sizeOfBufferInBytes, nullptr);
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂��B
ConstantBuffer* ConstantBuffer::Create(
	ID3D11Device* graphicsDevice,
	UINT byteWidth, void* source)
{
	// �C���f�b�N�X�o�b�t�@�[���쐬
	ID3D11Buffer* buffer = nullptr;

	// �쐬����C���f�b�N�X�o�b�t�@�[�ɂ��Ă̋L�q
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr;
	if (source == nullptr) {
		hr = graphicsDevice->CreateBuffer(&bufferDesc, nullptr, &buffer);
	}
	else {
		// ���������Ɏg�p���錳�f�[�^
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = source;
		hr = graphicsDevice->CreateBuffer(&bufferDesc, &initialData, &buffer);
	}
	if (FAILED(hr)) {
		OutputDebugString(L"���_�o�b�t�@�[�̍쐬�Ɏ��s���܂����B");
		return nullptr;
	}

	// �߂�l�Ƃ��Ă������C���X�^���X�𐶐�
	auto result = new(nothrow) ConstantBuffer();
	if (result == nullptr) {
		OutputDebugString(L"VertexBuffer���쐬����ۂɃ������[���s�����܂����B");
		return nullptr;
	}
	result->buffer = buffer;
	result->sizeInBytes = byteWidth;
	return result;
}

// ���\�[�X��������܂��B
void ConstantBuffer::Release()
{
	SAFE_RELEASE(buffer);
	delete this;
}

// D3D11�̃l�C�e�B�u�|�C���^�[���擾���܂��B
ID3D11Buffer* ConstantBuffer::GetNativePointer()
{
	return buffer;
}

// �w�肳�ꂽ�f�[�^���o�b�t�@�[�֏������݂܂��B
void ConstantBuffer::SetData(void* data)
{
	ID3D11Device* device = nullptr;
	buffer->GetDevice(&device);
	ID3D11DeviceContext* context = nullptr;
	device->GetImmediateContext(&context);

	context->UpdateSubresource(buffer, 0, NULL, data, 0, 0);

	SAFE_RELEASE(context);
	SAFE_RELEASE(device);
}

// �C���f�b�N�X�o�b�t�@�[�̃T�C�Y(�o�C�g�P��)���擾���܂��B
UINT ConstantBuffer::GetSizeInBytes()
{
	return sizeInBytes;
}
