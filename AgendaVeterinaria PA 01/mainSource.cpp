#include <Windows.h>
#include "resource.h"

HWND hMenu;
HINSTANCE hInstGlobal;

BOOL CALLBACK menuPrincipal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK nuevaCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK editarCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK editarInfoDoctor(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK pagarCita(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	hInstGlobal = hInst;
	hMenu = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MENU), NULL, menuPrincipal);
	ShowWindow(hMenu, cShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

BOOL CALLBACK menuPrincipal(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		MessageBox(hwnd, "Ventana creada correctamente.", "Mensaje.", MB_OK);
	}
		break;
	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			DestroyWindow(hMenu);
		}
		if (LOWORD(wParam) == BTN_MENU_EDITDOCINFO && HIWORD(wParam) == BN_CLICKED) {
			int edResult = DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_EDITDOCTOR), hwnd, editarInfoDoctor);
		}
		if (LOWORD(wParam) == BTN_NUEVACITA && HIWORD(wParam) == BN_CLICKED) {
			int ncResult = DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_NUEVACITA), hwnd, nuevaCita);
		}
		if (LOWORD(wParam) == BTN_PAGARCITA && HIWORD(wParam) == BN_CLICKED) {
			int pcResult = DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PAGOCITA), hwnd, pagarCita);
		}
		if (LOWORD(wParam) == BTN_EDITARCITA && HIWORD(wParam) == BN_CLICKED) {
			int ecResult = DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_EDITARCITA), hwnd, editarCita);
		}
	}
	    break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

BOOL CALLBACK nuevaCita(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_NC_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			EndDialog(hwnd, 0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK editarInfoDoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_EDM_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			EndDialog(hwnd, 0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK pagarCita(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_PC_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			EndDialog(hwnd, 0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK editarCita(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_EC_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			EndDialog(hwnd, 0);
		}
		break;
	}
	return FALSE;
}