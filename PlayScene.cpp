#include "PlayScene.h"
#include "stdafx.h"
#include "GameFramework.h"

#define MAP GetFramework()->GetCurMap()

CPlayScene::CPlayScene(HWND _hWnd, HINSTANCE _hInst, CGameFramework* GameFramework) : CScene(_hWnd, _hInst, GameFramework)
{

}

CPlayScene::~CPlayScene()
{
}

void CPlayScene::Initialize()
{
	// TODO : Bitmap, Map, player의 데이터를 Initialize 한다.
	
	MAP->Initialize(hInst);		// 선택된 Map의 Initialize

	// Resource
	backgroundImage = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STAGEFRAME));
}


void CPlayScene::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	GetKeyboardState(pKeysBuffer);

	bool stop = true;
	// TODO : player의 방향 벡터를 설정한다.
	if (pKeysBuffer[VK_UP] & 0xF0) MAP->Getplayer()->SetDirection(DIR_UP), stop = false;
	if (pKeysBuffer[VK_DOWN] & 0xF0) MAP->Getplayer()->SetDirection(DIR_DOWN), stop = false;
	if (pKeysBuffer[VK_LEFT] & 0xF0) MAP->Getplayer()->SetDirection(DIR_LEFT), stop = false;
	if (pKeysBuffer[VK_RIGHT] & 0xF0) MAP->Getplayer()->SetDirection(DIR_RIGHT), stop = false;
	if (pKeysBuffer[VK_SPACE] & 0xF0) MAP->SetBallon();	// TODO : player 물풍선 설치

	MAP->Getplayer()->SetStop(stop);
}

void CPlayScene::Update(float fTimeElapsed)
{
	// TODO : Play Scene Update
	// 1. player의 방향벡터에 따라 움직인다.
	// 2. Bitmap의 좌표를 움직여서 애니메이션을 넣는다.
	MAP->Update(fTimeElapsed);

	// 서버로 키 입력 버퍼 데이터들을 보낸다
}

void CPlayScene::Render()
{
	HDC hdc = GetDC(hWnd);

	HDC MemDC = CreateCompatibleDC(hdc);
	HDC MemDCImage = CreateCompatibleDC(hdc);

	hBit = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	OldBit[0] = (HBITMAP)SelectObject(MemDC, hBit);
	OldBit[1] = (HBITMAP)SelectObject(MemDCImage, backgroundImage); //--- 배경 이미지	
	TransparentBlt(MemDC, 0, 0, rc.right, rc.bottom, MemDCImage, 0, 0, 800, 600, RGB(255, 0, 255));

	MAP->Render(MemDC, MemDCImage);			// 선택된 Map을 Render 한다.
	//player->Render(MemDC, MemDCImage);		// player Render


	BitBlt(hdc, 0, 0, rc.right, rc.bottom, MemDC, 0, 0, SRCCOPY);
	// 자원 해제
	SelectObject(MemDC, OldBit[0]);
	DeleteObject(hBit);
	DeleteDC(MemDC);
	DeleteDC(MemDCImage);
	ReleaseDC(hWnd, hdc);
}

void CPlayScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&cursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}
