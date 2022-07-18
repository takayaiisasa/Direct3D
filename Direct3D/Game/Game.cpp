//=============================================================================
// Game.cpp
// 
//=============================================================================
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "Game.h"
#include "BasicVertexShader.h"	// �V�F�[�_�[���R���p�C�������w�b�_�[�t�@�C��
#include "BasicPixelShader.h"
#include "BasicGeometryShader.h"
#include <new>
using namespace std;
using namespace DirectX;

// �֐��̃v���g�^�C�v�錾
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// �A�v���P�[�V���������������܂��B
void Game::Initialize(LPCWSTR windowTitle, int screenWidth, int screenHeight)
{
	// �E�B���h�E�̃^�C�g��
	WindowTitle = windowTitle;
	// �E�B���h�E�̕�
	ScreenWidth = screenWidth;
	// �E�B���h�E�̍���
	ScreenHeight = screenHeight;
}

// �E�B���h�E���쐬���܂��B
bool Game::InitWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	// �E�B���h�E �N���X��o�^����
	const wchar_t CLASS_NAME[] = L"GameWindow";
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = WindowProc;	// �E�B���h�E �v���V�[�W���[���w��
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wndClass.lpszClassName = CLASS_NAME;
	if (!RegisterClassEx(&wndClass)) {
		return false;
	}

	// �N���C�A���g�̈悪�w�肵���𑜓x�ɂȂ�E�B���h�E�T�C�Y���v�Z
	RECT rect = { 0, 0, ScreenWidth, ScreenHeight };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	// �E�B���h�E���쐬����
	auto hWnd = CreateWindowEx(
		0, CLASS_NAME, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(rect.right - rect.left), (rect.bottom - rect.top),
		NULL, NULL, hInstance, NULL);
	if (hWnd == NULL) {
		return false;
	}

	// �E�B���h�E��\��
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	this->hWnd = hWnd;

	return true;
}

// �E�B���h�E ���b�Z�[�W����������v���V�[�W���[
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CLOSE:
		// �E�B���h�E�����
		if (MessageBox(hWnd, L"�E�B���h�E����܂����H", L"���b�Z�[�W", MB_OKCANCEL) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		// �A�v���P�[�V�������I��
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// �O���t�B�b�N�f�o�C�X���쐬���܂��B
bool Game::InitGraphicsDevice()
{
	HRESULT hr = S_OK;

	// �f�o�C�X�쐬���̃I�v�V�����t���O
	UINT creationFlags = 0;
#if defined(_DEBUG)
	// DEBUG�r���h�̍ۂ�Direct3D�̃f�o�b�O�\���@�\����������
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// �쐬����X���b�v�`�F�[���ɂ��Ă̋L�q
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = ScreenWidth;
	swapChainDesc.BufferDesc.Height = ScreenHeight;
	swapChainDesc.BufferDesc.RefreshRate = { 60, 1 };
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage =
		DXGI_USAGE_RENDER_TARGET_OUTPUT |
		DXGI_USAGE_SHADER_INPUT;	// �V�F�[�_�[���\�[�X�Ƃ��Ďg�p���邱�Ƃ�ݒ�
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	// �f�o�C�X�A�f�o�C�X�R���e�L�X�g�A�X���b�v�`�F�[�����쐬
	hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&swapChain, &graphicsDevice, &featureLevel, &immediateContext);
	if (FAILED(hr)) {
		return false;
	}

	// �o�b�N�o�b�t�@�[���擾
	ID3D11Texture2D* backBuffer = nullptr;
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		return false;
	}
	// �o�b�N�o�b�t�@�[�ɃA�N�Z�X���邽�߂̃����_�[�^�[�Q�b�g �r���[���쐬
	hr = graphicsDevice->CreateRenderTargetView(backBuffer, NULL, &renderTargetViews[0]);
	if (FAILED(hr)) {
		return false;
	}
	// �o�b�N�o�b�t�@�[�ɃV�F�[�_�[����A�N�Z�X���邽�߂̃��\�[�X �r���[���쐬
	hr = graphicsDevice->CreateShaderResourceView(
		backBuffer,
		NULL,
		&renderTargetResourceView);
	if (FAILED(hr)) {
		return false;
	}
	SAFE_RELEASE(backBuffer);

	// �e�N�X�`���ƃV�F�[�_�[���\�[�X�r���[�̃t�H�[�}�b�g��ݒ�
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
	// �[�x�X�e���V�����쐬
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
		D3D11_BIND_SHADER_RESOURCE;	// �V�F�[�_�[���\�[�X�Ƃ��Ďg�p���邱�Ƃ�ݒ�
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	hr = graphicsDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencil);
	if (FAILED(hr)) {
		return false;
	}
	// �[�x�X�e���V���ɃA�N�Z�X���邽�߂̃r���[���쐬
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
	// �[�x�X�e���V���ɃV�F�[�_�[����A�N�Z�X���邽�߂̃��\�[�X �r���[���쐬
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

	// �r���[�|�[�g
	viewports[0].Width = static_cast<FLOAT>(ScreenWidth);
	viewports[0].Height = static_cast<FLOAT>(ScreenHeight);
	viewports[0].MinDepth = 0.0f;
	viewports[0].MaxDepth = 1.0f;
	viewports[0].TopLeftX = 0.0f;
	viewports[0].TopLeftY = 0.0f;

	return true;
}

// �O���t�B�b�N���\�[�X��������܂��B
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

// ���b�Z�[�W���[�v�����s���܂��B
int Game::Run()
{
	// �E�B���h�E���쐬
	if (!InitWindow()) {
		MessageBox(NULL, L"�E�B���h�E���������ł��܂���ł����B", L"���b�Z�[�W", MB_OK);
		return -1;
	}
	// �O���t�B�b�N�f�o�C�X���쐬
	if (!InitGraphicsDevice()) {
		MessageBox(NULL, L"�O���t�B�b�N�f�o�C�X���������ł��܂���ł����B", L"���b�Z�[�W", MB_OK);
		return -1;
	}

	// ���_�f�[�^�̔z��
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
	// ���_�o�b�t�@�[���쐬
	VertexBuffer* vertexBuffer = VertexBuffer::Create(
		graphicsDevice,
		vertices, sizeof vertices);
	if (vertexBuffer == nullptr) {
		OutputDebugString(L"���_�o�b�t�@�[���쐬�ł��܂���ł����B");
		return -1;
	}

	// �C���f�b�N�X�f�[�^�̔z��
	UINT32 indices[] = {
		 0,  1,  2,  3,  2,  1,	// Front
		 4,  5,  6,  7,  6,  5,	// Back
		 8,  9, 10, 11, 10,  9,	// Right
		12, 13, 14, 15, 14, 13,	// Left
		16, 17, 18, 19, 18, 17,	// Top
		20, 21, 22, 23, 22, 21,	// Bottom
	};
	// �C���f�b�N�X�o�b�t�@�[���쐬
	IndexBuffer* indexBuffer = IndexBuffer::Create(
		graphicsDevice, sizeof indices);
	if (indexBuffer == nullptr) {
		OutputDebugString(L"�C���f�b�N�X�o�b�t�@�[���쐬�ł��܂���ł����B");
		return -1;
	}
	indexBuffer->SetData(indices);

	// �萔�o�b�t�@�[����ăV�F�[�_�[�ɖ��t���[������s��f�[�^��\���܂��B
	struct MatricesPerFrame {
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projectionMatrix;
		DirectX::XMFLOAT4X4 worldViewProjectionMatrix;
		// �J�����̈ʒu���W
		DirectX::XMFLOAT4 viewPosition = DirectX::XMFLOAT4(0, 1, -10, 1);
		// ���C�g�̈ʒu���W(���s���� w = 0, �_���� w = 1)
		DirectX::XMFLOAT4 lightPosition = DirectX::XMFLOAT4(1.0f, 2.0f, -2.0f, 1.0f);
		// �}�e���A���̕\�ʃJ���[
		DirectX::XMFLOAT4 materialDiffuse = DirectX::XMFLOAT4(1, 1, 0, 1);

		// ���ʔ��˂̐F(r, g, b) = (x, y, z)
		DirectX::XMFLOAT3 materialSpecularColor = DirectX::XMFLOAT3(1, 1, 1);
		// ���ʔ��˂̋���(float) = w
		float materialSpecularPower = 1;
	};

	// �萔�o�b�t�@�[���쐬
	ConstantBuffer* constantBuffer = ConstantBuffer::Create(graphicsDevice, sizeof(MatricesPerFrame));
	if (constantBuffer == nullptr) {
		OutputDebugString(L"�萔�o�b�t�@�[���쐬�ł��܂���ł����B");
		return -1;
	}

	// �V�F�[�_�[���쐬
	BasicVertexShader* vertexShader = BasicVertexShader::Create(graphicsDevice);
	if (vertexShader == nullptr) {
		OutputDebugString(L"���_�V�F�[�_�[���쐬�ł��܂���ł����B");
	}
	BasicGeometryShader* geometryShader = BasicGeometryShader::Create(graphicsDevice);
	if (geometryShader == nullptr) {
		OutputDebugString(L"�W�I���g���[�V�F�[�_�[���쐬�ł��܂���ł����B");
	}
	BasicPixelShader* pixelShader = BasicPixelShader::Create(graphicsDevice);
	if (pixelShader == nullptr) {
		OutputDebugString(L"�s�N�Z���V�F�[�_�[���쐬�ł��܂���ł����B");
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// ���̓��C�A�E�g���쐬
	InputLayout* inputLayout = InputLayout::Create(graphicsDevice,
		inputElementDescs, ARRAYSIZE(inputElementDescs),
		g_BasicVertexShader, ARRAYSIZE(g_BasicVertexShader));
	if (inputLayout == nullptr) {
		OutputDebugString(L"���̓��C�A�E�g���쐬�ł��܂���ł����B");
		return -1;
	}


	// �摜�f�[�^�̃_�~�[
	const uint32_t source[] = {
		0xFF00FFFF, 0xFF000000, 0xFF00FFFF, 0xFF000000,
		0xFF000000, 0xFF00FFFF, 0xFF000000, 0xFF00FFFF,
		0xFF00FFFF, 0xFF000000, 0xFF00FFFF, 0xFF000000,
		0xFF000000, 0xFF00FFFF, 0xFF000000, 0xFF00FFFF,
	};

	// �e�N�X�`���[���쐬
	Texture2D* texture =
		Texture2D::Create(graphicsDevice, 4, 4, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, false);
	if (texture == nullptr) {
		OutputDebugString(L"�e�N�X�`���[���쐬�ł��܂���ł����B");
		return -1;
	}
	// �s�N�Z���f�[�^���X�V
	texture->SetData(source);


	float time = 0;

	// ���b�Z�[�W���[�v�����s
	MSG msg = {};
	while (true) {
		time += 0.01666f;

		// �萔�o�b�t�@�[�֓]������f�[�^�\�[�X������

		// ���[���h�ϊ��s��
		XMMATRIX worldMatrix = XMMatrixIdentity();
		worldMatrix *= XMMatrixScaling(1.0f, 1.0f, 1.0f);
		XMVECTOR axis = XMVectorSet(1, 1, 0, 0);
		worldMatrix *= XMMatrixRotationAxis(axis, time);
		worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		// �r���[�s����v�Z
		XMVECTOR eyePosition = XMVectorSet(0, 5, -10, 1);
		XMVECTOR focusPosition = XMVectorSet(0, 0, 0, 1);
		XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
		XMMATRIX viewMatrix =
			XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
		// �v���W�F�N�V�����s����v�Z
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
		// ���C�g
		matricesPerFrame.lightPosition = DirectX::XMFLOAT4(1.0f, 2.0f, -2.0f, 1.0f);
		// �}�e���A��
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
		// �萔�o�b�t�@�[���X�V
		constantBuffer->SetData(&matricesPerFrame);

		// �����_�[�^�[�Q�b�g��ݒ�
		immediateContext->OMSetRenderTargets(1, renderTargetViews, depthStencilView);
		// ��ʂ��N���A�[
		immediateContext->ClearRenderTargetView(renderTargetViews[0], clearColor);
		immediateContext->ClearDepthStencilView(depthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		// �r���[�|�[�g��ݒ�
		immediateContext->RSSetViewports(1, viewports);

		// ���_�o�b�t�@�[��ݒ�
		ID3D11Buffer* vertexBuffers[1] = { vertexBuffer->GetNativePointer() };
		UINT strides[1] = { sizeof(VertexPositionNormalTexture) };
		UINT offsets[1] = { 0 };
		immediateContext->IASetVertexBuffers(
			0,
			ARRAYSIZE(vertexBuffers),
			vertexBuffers, strides, offsets);

		// �V�F�[�_�[��ݒ�
		immediateContext->VSSetShader(vertexShader->GetNativePointer(), NULL, 0);
		immediateContext->GSSetShader(geometryShader->GetNativePointer(), NULL, 0);
		immediateContext->PSSetShader(pixelShader->GetNativePointer(), NULL, 0);

		// ���_�V�F�[�_�[�ɒ萔�o�b�t�@�[��ݒ�
		ID3D11Buffer* constantBuffers[1] = { constantBuffer->GetNativePointer() };
		immediateContext->VSSetConstantBuffers(0, 1, constantBuffers);
		immediateContext->GSSetConstantBuffers(0, 1, constantBuffers);
		immediateContext->PSSetConstantBuffers(0, 1, constantBuffers);

		ID3D11ShaderResourceView* textureViews[1] = { texture->GetShaderResourceView() };
		immediateContext->PSSetShaderResources(0, 1, textureViews);
		ID3D11SamplerState* samplerStates[1] = { texture->GetSamplerState() };
		immediateContext->PSSetSamplers(0, 1, samplerStates);

		// ���_�o�b�t�@�[�ƒ��_�V�F�[�_�[�̑g�����ɑΉ��������̓��C�A�E�g��ݒ�
		immediateContext->IASetInputLayout(inputLayout->GetNativePointer());
		// �v���~�e�B�u�g�|���W�[�Ƃ��ăg���C�A���O����ݒ�
		immediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �C���f�b�N�X�o�b�t�@�[��ݒ�
		immediateContext->IASetIndexBuffer(indexBuffer->GetNativePointer(), DXGI_FORMAT_R32_UINT, 0);

		// �`��
		immediateContext->DrawIndexed(ARRAYSIZE(indices), 0, 0);

		// �o�b�N�o�b�t�@�[�ɕ`�悵���C���[�W���f�B�X�v���C�ɕ\��
		HRESULT hr = S_OK;
		hr = swapChain->Present(1, 0);
		if (FAILED(hr))
		{
			MessageBox(hWnd,
				L"�O���t�B�b�N�f�o�C�X�������I�Ɏ��O���ꂽ���A�h���C�o�[���A�b�v�f�[�g����܂����B",
				L"�G���[", MB_OK);
			return -1;
		}

		// ���̃E�B���h�E�̃��b�Z�[�W�����݂��邩���m�F
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			// ���b�Z�[�W���擾
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// �������
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
