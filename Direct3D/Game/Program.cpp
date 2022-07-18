//=============================================================================
// Program.cpp
// �G���g���[�|�C���g
//=============================================================================

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Game.h"

// �A�v���P�[�V�����̃G���g���[�|�C���g
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// �f�o�b�O�r���h�̏ꍇ�̂݃������[���[�N�����o����
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Game game;
	// �A�v���P�[�V�����̋N���ݒ�
	game.Initialize(L"Game�^�C�g��", 640, 480);
	// ���b�Z�[�W ���[�v�����s
	return game.Run();
}
