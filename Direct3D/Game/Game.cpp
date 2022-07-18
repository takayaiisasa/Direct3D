//=============================================================================
// Game.cpp
// 
//=============================================================================
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "Game.h"
#include "BasicVertexShader.h"	// シェーダーをコンパイルしたヘッダーファイル
#include "BasicPixelShader.h"
#include "BasicGeometryShader.h"
#include <new>
using namespace std;
using namespace DirectX;

// 関数のプロトタイプ宣言
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// アプリケーションを初期化します。
void Game::Initialize(LPCWSTR windowTitle, int screenWidth, int screenHeight)
{
	// ウィンドウのタイトル
	WindowTitle = windowTitle;
	// ウィンドウの幅
	ScreenWidth = screenWidth;
	// ウィンドウの高さ
	ScreenHeight = screenHeight;
}

// ウィンドウを作成します。
bool Game::InitWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	// ウィンドウ クラスを登録する
	const wchar_t CLASS_NAME[] = L"GameWindow";
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = WindowProc;	// ウィンドウ プロシージャーを指定
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wndClass.lpszClassName = CLASS_NAME;
	if (!RegisterClassEx(&wndClass)) {
		return false;
	}

	// クライアント領域が指定した解像度になるウィンドウサイズを計算
	RECT rect = { 0, 0, ScreenWidth, ScreenHeight };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	// ウィンドウを作成する
	auto hWnd = CreateWindowEx(
		0, CLASS_NAME, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(rect.right - rect.left), (rect.bottom - rect.top),
		NULL, NULL, hInstance, NULL);
	if (hWnd == NULL) {
		return false;
	}

	// ウィンドウを表示
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	this->hWnd = hWnd;

	return true;
}

// ウィンドウ メッセージを処理するプロシージャー
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CLOSE:
		// ウィンドウを閉じる
		if (MessageBox(hWnd, L"ウィンドウを閉じますか？", L"メッセージ", MB_OKCANCEL) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		// アプリケーションを終了
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// グラフィックデバイスを作成します。
bool Game::InitGraphicsDevice()
{
	HRESULT hr = S_OK;

	// デバイス作成時のオプションフラグ
	UINT creationFlags = 0;
#if defined(_DEBUG)
	// DEBUGビルドの際にDirect3Dのデバッグ表示機能を持たせる
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 作成するスワップチェーンについての記述
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = ScreenWidth;
	swapChainDesc.BufferDesc.Height = ScreenHeight;
	swapChainDesc.BufferDesc.RefreshRate = { 60, 1 };
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage =
		DXGI_USAGE_RENDER_TARGET_OUTPUT |
		DXGI_USAGE_SHADER_INPUT;	// シェーダーリソースとして使用することを設定
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	// デバイス、デバイスコンテキスト、スワップチェーンを作成
	hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&swapChain, &graphicsDevice, &featureLevel, &immediateContext);
	if (FAILED(hr)) {
		return false;
	}

	// バックバッファーを取得
	ID3D11Texture2D* backBuffer = nullptr;
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		return false;
	}
	// バックバッファーにアクセスするためのレンダーターゲット ビューを作成
	hr = graphicsDevice->CreateRenderTargetView(backBuffer, NULL, &renderTargetViews[0]);
	if (FAILED(hr)) {
		return false;
	}
	// バックバッファーにシェーダーからアクセスするためのリソース ビューを作成
	hr = graphicsDevice->CreateShaderResourceView(
		backBuffer,
		NULL,
		&renderTargetResourceView);
	if (FAILED(hr)) {
		return false;
	}
	SAFE_RELEASE(backBuffer);

	// テクスチャとシェーダーリソースビューのフォーマットを設定
	DXGI_FORMAT textureFormat = depthStencilFormat;
	DXGI_FORMAT resourceFormat = depthStencilFormat;
	switch (depthStencilFormat)
	{
	case DXGI_FORMAT_D16_UNORM:
		textureFormat = DXGI_FORMAT_R16_TYPELESS;
		resourceFormat = DXGI_FORMAT_R16_UNORM;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		textureFormat = DXGI_FORMAT_R24G8_TYPELESS;
		resourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT:
		textureFormat = DXGI_FORMAT_R32_TYPELESS;
		resourceFormat = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		textureFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
		resourceFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	}
	// 深度ステンシルを作成
	ID3D11Texture2D* depthStencil = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = swapChainDesc.BufferDesc.Width;
	depthStencilDesc.Height = swapChainDesc.BufferDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = textureFormat;
	depthStencilDesc.SampleDesc = swapChainDesc.SampleDesc;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags =
		D3D11_BIND_DEPTH_STENCIL |
		D3D11_BIND_SHADER_RESOURCE;	// シェーダーリソースとして使用することを設定
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	hr = graphicsDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencil);
	if (FAILED(hr)) {
		return false;
	}
	// 深度ステンシルにアクセスするためのビューを作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depthStencilFormat;
	if (depthStencilDesc.SampleDesc.Count > 0) {
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}
	else {
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
	}
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = graphicsDevice->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(hr)) {
		return false;
	}
	// 深度ステンシルにシェーダーからアクセスするためのリソース ビューを作成
	D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilResourceViewDesc = {};
	depthStencilResourceViewDesc.Format = resourceFormat;
	if (depthStencilDesc.SampleDesc.Count > 0) {
		depthStencilResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	}
	else {
		depthStencilResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthStencilResourceViewDesc.Texture2D.MostDetailedMip = 0;
		depthStencilResourceViewDesc.Texture2D.MipLevels = 1;
	}
	hr = graphicsDevice->CreateShaderResourceView(
		depthStencil,
		&depthStencilResourceViewDesc,
		&depthStencilResourceView);
	if (FAILED(hr)) {
		return false;
	}
	SAFE_RELEASE(depthStencil);

	// ビューポート
	viewports[0].Width = static_cast<FLOAT>(ScreenWidth);
	viewports[0].Height = static_cast<FLOAT>(ScreenHeight);
	viewports[0].MinDepth = 0.0f;
	viewports[0].MaxDepth = 1.0f;
	viewports[0].TopLeftX = 0.0f;
	viewports[0].TopLeftY = 0.0f;

	return true;
}

// グラフィックリソースを解放します。
void Game::ReleaseGraphicsDevice()
{
	SAFE_RELEASE(depthStencilResourceView);
	SAFE_RELEASE(depthStencilView);
	SAFE_RELEASE(renderTargetResourceView);
	SAFE_RELEASE(renderTargetViews[0]);
	SAFE_RELEASE(swapChain);
	SAFE_RELEASE(immediateContext);
	SAFE_RELEASE(graphicsDevice);
}

// メッセージループを実行します。
int Game::Run()
{
	// ウィンドウを作成
	if (!InitWindow()) {
		MessageBox(NULL, L"ウィンドウを初期化できませんでした。", L"メッセージ", MB_OK);
		return -1;
	}
	// グラフィックデバイスを作成
	if (!InitGraphicsDevice()) {
		MessageBox(NULL, L"グラフィックデバイスを初期化できませんでした。", L"メッセージ", MB_OK);
		return -1;
	}

	// 頂点データの配列
	VertexPositionNormalTexture vertices[] = {
		// Front
		{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ {-1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
		{ { 1.0f,-1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
		{ {-1.0f,-1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		// Back
		{ {-1.0f, 1.0f,-1.0f }, { 0.0f, 0.0f,-1.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 1.0f,-1.0f }, { 0.0f, 0.0f,-1.0f }, { 1.0f, 0.0f } },
		{ {-1.0f,-1.0f,-1.0f }, { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f } },
		{ { 1.0f,-1.0f,-1.0f }, { 0.0f, 0.0f,-1.0f }, { 1.0f, 1.0f } },
		// Right
		{ { 1.0f, 1.0f,-1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 1.0f,-1.0f,-1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { 1.0f,-1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		// Left
		{ {-1.0f, 1.0f, 1.0f }, {-1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {-1.0f, 1.0f,-1.0f }, {-1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {-1.0f,-1.0f, 1.0f }, {-1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {-1.0f,-1.0f,-1.0f }, {-1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		// UP
		{ {-1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {-1.0f, 1.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { 1.0f, 1.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
		// DOWN
		{ { 1.0f,-1.0f, 1.0f }, { 0.0f,-1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {-1.0f,-1.0f, 1.0f }, { 0.0f,-1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 1.0f,-1.0f,-1.0f }, { 0.0f,-1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {-1.0f,-1.0f,-1.0f }, { 0.0f,-1.0f, 0.0f }, { 1.0f, 1.0f } },
	};
	// 頂点バッファーを作成
	VertexBuffer* vertexBuffer = VertexBuffer::Create(
		graphicsDevice,
		vertices, sizeof vertices);
	if (vertexBuffer == nullptr) {
		OutputDebugString(L"頂点バッファーを作成できませんでした。");
		return -1;
	}

	// インデックスデータの配列
	UINT32 indices[] = {
		 0,  1,  2,  3,  2,  1,	// Front
		 4,  5,  6,  7,  6,  5,	// Back
		 8,  9, 10, 11, 10,  9,	// Right
		12, 13, 14, 15, 14, 13,	// Left
		16, 17, 18, 19, 18, 17,	// Top
		20, 21, 22, 23, 22, 21,	// Bottom
	};
	// インデックスバッファーを作成
	IndexBuffer* indexBuffer = IndexBuffer::Create(
		graphicsDevice, sizeof indices);
	if (indexBuffer == nullptr) {
		OutputDebugString(L"インデックスバッファーを作成できませんでした。");
		return -1;
	}
	indexBuffer->SetData(indices);

	// 定数バッファーを介してシェーダーに毎フレーム送る行列データを表します。
	struct MatricesPerFrame {
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projectionMatrix;
		DirectX::XMFLOAT4X4 worldViewProjectionMatrix;
		// カメラの位置座標
		DirectX::XMFLOAT4 viewPosition = DirectX::XMFLOAT4(0, 1, -10, 1);
		// ライトの位置座標(平行光源 w = 0, 点光源 w = 1)
		DirectX::XMFLOAT4 lightPosition = DirectX::XMFLOAT4(1.0f, 2.0f, -2.0f, 1.0f);
		// マテリアルの表面カラー
		DirectX::XMFLOAT4 materialDiffuse = DirectX::XMFLOAT4(1, 1, 0, 1);

		// 鏡面反射の色(r, g, b) = (x, y, z)
		DirectX::XMFLOAT3 materialSpecularColor = DirectX::XMFLOAT3(1, 1, 1);
		// 鏡面反射の強さ(float) = w
		float materialSpecularPower = 1;
	};

	// 定数バッファーを作成
	ConstantBuffer* constantBuffer = ConstantBuffer::Create(graphicsDevice, sizeof(MatricesPerFrame));
	if (constantBuffer == nullptr) {
		OutputDebugString(L"定数バッファーを作成できませんでした。");
		return -1;
	}

	// シェーダーを作成
	BasicVertexShader* vertexShader = BasicVertexShader::Create(graphicsDevice);
	if (vertexShader == nullptr) {
		OutputDebugString(L"頂点シェーダーを作成できませんでした。");
	}
	BasicGeometryShader* geometryShader = BasicGeometryShader::Create(graphicsDevice);
	if (geometryShader == nullptr) {
		OutputDebugString(L"ジオメトリーシェーダーを作成できませんでした。");
	}
	BasicPixelShader* pixelShader = BasicPixelShader::Create(graphicsDevice);
	if (pixelShader == nullptr) {
		OutputDebugString(L"ピクセルシェーダーを作成できませんでした。");
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// 入力レイアウトを作成
	InputLayout* inputLayout = InputLayout::Create(graphicsDevice,
		inputElementDescs, ARRAYSIZE(inputElementDescs),
		g_BasicVertexShader, ARRAYSIZE(g_BasicVertexShader));
	if (inputLayout == nullptr) {
		OutputDebugString(L"入力レイアウトを作成できませんでした。");
		return -1;
	}


	// 画像データのダミー
	const uint32_t source[] = {
		0xFF00FFFF, 0xFF000000, 0xFF00FFFF, 0xFF000000,
		0xFF000000, 0xFF00FFFF, 0xFF000000, 0xFF00FFFF,
		0xFF00FFFF, 0xFF000000, 0xFF00FFFF, 0xFF000000,
		0xFF000000, 0xFF00FFFF, 0xFF000000, 0xFF00FFFF,
	};

	// テクスチャーを作成
	Texture2D* texture =
		Texture2D::Create(graphicsDevice, 4, 4, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, false);
	if (texture == nullptr) {
		OutputDebugString(L"テクスチャーを作成できませんでした。");
		return -1;
	}
	// ピクセルデータを更新
	texture->SetData(source);


	float time = 0;

	// メッセージループを実行
	MSG msg = {};
	while (true) {
		time += 0.01666f;

		// 定数バッファーへ転送するデータソースを準備

		// ワールド変換行列
		XMMATRIX worldMatrix = XMMatrixIdentity();
		worldMatrix *= XMMatrixScaling(1.0f, 1.0f, 1.0f);
		XMVECTOR axis = XMVectorSet(1, 1, 0, 0);
		worldMatrix *= XMMatrixRotationAxis(axis, time);
		worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		// ビュー行列を計算
		XMVECTOR eyePosition = XMVectorSet(0, 5, -10, 1);
		XMVECTOR focusPosition = XMVectorSet(0, 0, 0, 1);
		XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
		XMMATRIX viewMatrix =
			XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
		// プロジェクション行列を計算
		float fovAngleY = 60.0f;
		float aspectRatio = 640 / 480.0f;
		float nearZ = 0.3f;
		float farZ = 1000.0f;
		XMMATRIX projectionMatrix =
			XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngleY), aspectRatio, nearZ, farZ);

		MatricesPerFrame matricesPerFrame = {};
		XMStoreFloat4x4(
			&matricesPerFrame.worldMatrix,
			XMMatrixTranspose(worldMatrix));
		XMStoreFloat4x4(
			&matricesPerFrame.viewMatrix,
			XMMatrixTranspose(viewMatrix));
		XMStoreFloat4x4(
			&matricesPerFrame.projectionMatrix,
			XMMatrixTranspose(projectionMatrix));
		XMStoreFloat4x4(
			&matricesPerFrame.worldViewProjectionMatrix,
			XMMatrixTranspose(worldMatrix * viewMatrix * projectionMatrix));
		// ライト
		matricesPerFrame.lightPosition = DirectX::XMFLOAT4(1.0f, 2.0f, -2.0f, 1.0f);
		// マテリアル
		if (GetAsyncKeyState('D')) {
			//matricesPerFrame.materialDiffuse = DirectX::XMFLOAT4(1, 1, 0, 1);
			matricesPerFrame.materialDiffuse = DirectX::XMFLOAT4(1, 1, 1, 1);
		}
		else {
			matricesPerFrame.materialDiffuse = DirectX::XMFLOAT4(0, 0, 0, 1);
		}
		if (GetAsyncKeyState('S')) {
			matricesPerFrame.materialSpecularColor = DirectX::XMFLOAT3(1, 1, 1);
			matricesPerFrame.materialSpecularPower = 0.1f;
		}
		else {
			matricesPerFrame.materialSpecularColor = DirectX::XMFLOAT3(0, 0, 0);
		}
		// 定数バッファーを更新
		constantBuffer->SetData(&matricesPerFrame);

		// レンダーターゲットを設定
		immediateContext->OMSetRenderTargets(1, renderTargetViews, depthStencilView);
		// 画面をクリアー
		immediateContext->ClearRenderTargetView(renderTargetViews[0], clearColor);
		immediateContext->ClearDepthStencilView(depthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		// ビューポートを設定
		immediateContext->RSSetViewports(1, viewports);

		// 頂点バッファーを設定
		ID3D11Buffer* vertexBuffers[1] = { vertexBuffer->GetNativePointer() };
		UINT strides[1] = { sizeof(VertexPositionNormalTexture) };
		UINT offsets[1] = { 0 };
		immediateContext->IASetVertexBuffers(
			0,
			ARRAYSIZE(vertexBuffers),
			vertexBuffers, strides, offsets);

		// シェーダーを設定
		immediateContext->VSSetShader(vertexShader->GetNativePointer(), NULL, 0);
		immediateContext->GSSetShader(geometryShader->GetNativePointer(), NULL, 0);
		immediateContext->PSSetShader(pixelShader->GetNativePointer(), NULL, 0);

		// 頂点シェーダーに定数バッファーを設定
		ID3D11Buffer* constantBuffers[1] = { constantBuffer->GetNativePointer() };
		immediateContext->VSSetConstantBuffers(0, 1, constantBuffers);
		immediateContext->GSSetConstantBuffers(0, 1, constantBuffers);
		immediateContext->PSSetConstantBuffers(0, 1, constantBuffers);

		ID3D11ShaderResourceView* textureViews[1] = { texture->GetShaderResourceView() };
		immediateContext->PSSetShaderResources(0, 1, textureViews);
		ID3D11SamplerState* samplerStates[1] = { texture->GetSamplerState() };
		immediateContext->PSSetSamplers(0, 1, samplerStates);

		// 頂点バッファーと頂点シェーダーの組合せに対応した入力レイアウトを設定
		immediateContext->IASetInputLayout(inputLayout->GetNativePointer());
		// プリミティブトポロジーとしてトライアングルを設定
		immediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// インデックスバッファーを設定
		immediateContext->IASetIndexBuffer(indexBuffer->GetNativePointer(), DXGI_FORMAT_R32_UINT, 0);

		// 描画
		immediateContext->DrawIndexed(ARRAYSIZE(indices), 0, 0);

		// バックバッファーに描画したイメージをディスプレイに表示
		HRESULT hr = S_OK;
		hr = swapChain->Present(1, 0);
		if (FAILED(hr))
		{
			MessageBox(hWnd,
				L"グラフィックデバイスが物理的に取り外されたか、ドライバーがアップデートされました。",
				L"エラー", MB_OK);
			return -1;
		}

		// このウィンドウのメッセージが存在するかを確認
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			// メッセージを取得
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 解放処理
	texture->Release();
	inputLayout->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	constantBuffer->Release();
	vertexShader->Release();
	geometryShader->Release();
	pixelShader->Release();

	ReleaseGraphicsDevice();

	return (int)msg.wParam;
}
