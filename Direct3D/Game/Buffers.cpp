//=============================================================================
// Buffers.cpp
// 
//=============================================================================
#include "Game.h"
#include <new>
using namespace std;

// このクラスの新しいインスタンスを作成します。
VertexBuffer* VertexBuffer::Create(
	ID3D11Device* graphicsDevice,
	void* vertices, UINT byteWidth)
{
	// 頂点バッファーを作成
	ID3D11Buffer* buffer = nullptr;

	// 作成する頂点バッファーについての記述
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;	// 作成するバッファーのサイズ(bytes)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;	// バッファーの使用方法（とりあえずDEFAULT）
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// Vertex Bufferとして利用する
	bufferDesc.CPUAccessFlags = 0;			// CPUからの読み書きに使わない場合は0
	bufferDesc.MiscFlags = 0;				// オプションのフラグ
	bufferDesc.StructureByteStride = 0;		// 頂点バッファーとして使うなら0
	HRESULT hr;
	if (vertices == nullptr) {
		hr = graphicsDevice->CreateBuffer(&bufferDesc, nullptr, &buffer);
	}
	else {
		// 初期化時に使用する元データ
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = vertices;
		hr = graphicsDevice->CreateBuffer(&bufferDesc, &initialData, &buffer);
	}
	if (FAILED(hr)) {
		OutputDebugString(L"頂点バッファーの作成に失敗しました。");
		return nullptr;
	}

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) VertexBuffer();
	if (result == nullptr) {
		OutputDebugString(L"VertexBufferを作成する際にメモリーが不足しました。");
		return nullptr;
	}
	result->buffer = buffer;
	return result;
}

// リソースを解放します。
void VertexBuffer::Release() {
	SAFE_RELEASE(buffer);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11Buffer* VertexBuffer::GetNativePointer() {
	return buffer;
}

// このクラスの新しいインスタンスを作成します。
IndexBuffer* IndexBuffer::Create(
	ID3D11Device* device, UINT sizeOfBufferInBytes)
{
	return Create(device, sizeOfBufferInBytes, nullptr);
}

// このクラスの新しいインスタンスを作成します。
IndexBuffer* IndexBuffer::Create(
	ID3D11Device* graphicsDevice,
	UINT byteWidth, void* source)
{
	// インデックスバッファーを作成
	ID3D11Buffer* buffer = nullptr;

	// 作成するインデックスバッファーについての記述
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
		// 初期化時に使用する元データ
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = source;
		hr = graphicsDevice->CreateBuffer(&bufferDesc, &initialData, &buffer);
	}
	if (FAILED(hr)) {
		OutputDebugString(L"頂点バッファーの作成に失敗しました。");
		return nullptr;
	}

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) IndexBuffer();
	if (result == nullptr) {
		OutputDebugString(L"VertexBufferを作成する際にメモリーが不足しました。");
		return nullptr;
	}
	result->buffer = buffer;
	result->sizeInBytes = byteWidth;
	return result;
}

// リソースを解放します。
void IndexBuffer::Release()
{
	SAFE_RELEASE(buffer);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11Buffer* IndexBuffer::GetNativePointer()
{
	return buffer;
}

// 指定されたデータをバッファーへ書き込みます。
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

// インデックスバッファーのサイズ(バイト単位)を取得します。
UINT IndexBuffer::GetSizeInBytes()
{
	return sizeInBytes;
}

// このクラスの新しいインスタンスを作成します。
ConstantBuffer* ConstantBuffer::Create(
	ID3D11Device* device, UINT sizeOfBufferInBytes)
{
	return Create(device, sizeOfBufferInBytes, nullptr);
}

// このクラスの新しいインスタンスを作成します。
ConstantBuffer* ConstantBuffer::Create(
	ID3D11Device* graphicsDevice,
	UINT byteWidth, void* source)
{
	// インデックスバッファーを作成
	ID3D11Buffer* buffer = nullptr;

	// 作成するインデックスバッファーについての記述
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
		// 初期化時に使用する元データ
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = source;
		hr = graphicsDevice->CreateBuffer(&bufferDesc, &initialData, &buffer);
	}
	if (FAILED(hr)) {
		OutputDebugString(L"頂点バッファーの作成に失敗しました。");
		return nullptr;
	}

	// 戻り値としてかえすインスタンスを生成
	auto result = new(nothrow) ConstantBuffer();
	if (result == nullptr) {
		OutputDebugString(L"VertexBufferを作成する際にメモリーが不足しました。");
		return nullptr;
	}
	result->buffer = buffer;
	result->sizeInBytes = byteWidth;
	return result;
}

// リソースを解放します。
void ConstantBuffer::Release()
{
	SAFE_RELEASE(buffer);
	delete this;
}

// D3D11のネイティブポインターを取得します。
ID3D11Buffer* ConstantBuffer::GetNativePointer()
{
	return buffer;
}

// 指定されたデータをバッファーへ書き込みます。
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

// インデックスバッファーのサイズ(バイト単位)を取得します。
UINT ConstantBuffer::GetSizeInBytes()
{
	return sizeInBytes;
}
