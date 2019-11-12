#include <Windows.h>
#include <string>
#include <time.h>
#include "resource.h"
using namespace std;

#define TM_RELOJ 3000
#define TM_COMP 3001

time_t allTime;
struct tm *tiempoActual;
HWND hLblReloj;

struct CITA {
	CITA *prev;
	string nombreDueño;
	string nombreMascota;
	string telefono;
	string especie;
	string motivoConsulta;
	string fecha;
	string hora;
	HBITMAP foto1;
	HBITMAP foto2;
	int formaPago;
	float costo;
	CITA *next;
}*origin, *aux;

HWND hMenu;
HINSTANCE hInstGlobal;

HWND hLbAgenda;

bool verificarNum(string c);
bool verificarAlfa(string c);

BOOL CALLBACK menuPrincipal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK nuevaCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK editarCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK editarInfoDoctor(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK pagarCita(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	origin = aux = NULL;
	hInstGlobal = hInst;

	hMenu = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MENU), NULL, menuPrincipal);
	SetTimer(hMenu, TM_RELOJ, 1000, NULL);

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
		hLbAgenda = GetDlgItem(hwnd, IDC_LISTACITAS);
		hLblReloj = GetDlgItem(hwnd, ST_MENU_RELOJ);
	}break;
	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			KillTimer(hwnd, TM_RELOJ);
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
	}break;
	case WM_TIMER: {
			time(&allTime);
			tiempoActual = localtime(&allTime);
			char reloj[80];
			strftime(reloj, 80, "Hoy es: %d/%m/%Y  %I:%M:%S", tiempoActual);
			SetWindowText(hLblReloj, reloj);
		}break;
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
	case WM_INITDIALOG: {
		    SetTimer(hwnd, TM_COMP, 1, NULL);
			if (origin == NULL) {
				origin = new CITA;
				aux = origin;
				aux->next = NULL;
				aux->prev = NULL;
			}
			else {
				while (aux->next != NULL) {
					aux = aux->next;
				}
				aux->next = new CITA;
				aux->next->prev = aux;
				aux = aux->next;
				aux->next = NULL;
			}

			HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
			SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Perro");
			SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Gato");
			SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Loro");
			SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Tortuga");
			SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Pez");
	}break;
	case WM_COMMAND: {
		char buff[256];
		char buffName[50];
		if (LOWORD(wParam) == ID_NC_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			if (aux->next == NULL && aux->prev == NULL) {
				delete aux;
				aux = origin = NULL;
			}
			else {
				aux->prev->next = NULL;
				delete aux;
				aux = origin;
			}
			KillTimer(hwnd, TM_COMP);
			EndDialog(hwnd, 0);
		}
		if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
			HWND hEdNombreD = GetDlgItem(hwnd, EDT_NC_DNOMBRE);
			HWND hEdNombreM = GetDlgItem(hwnd, EDT_NC_MNOMBRE);
			HWND hEdTel = GetDlgItem(hwnd, EDT_NC_TELEFONO);
			HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
			HWND hTpFecha = GetDlgItem(hwnd, IDC_DATETIMEPICKER1);
			HWND hTpHora = GetDlgItem(hwnd, IDC_DATETIMEPICKER2);
			HWND hEdMotivo = GetDlgItem(hwnd, EDT_NC_MOTIVO);
			HWND hEdCosto = GetDlgItem(hwnd, EDT_NC_COSTO);
			int length;
			bool correcto = true;

			length = GetWindowTextLength(hEdNombreD);
			if (length > 0) {
				GetWindowText(hEdNombreD, buff, length+1);
				string s(buff);
				if (verificarNum(s)) {
					MessageBox(hwnd, "Nombre de Dueño no deben contener números.", "Aviso", MB_ICONEXCLAMATION);
					correcto = false;
				}
				else {
					aux->nombreDueño = s;
				}
			}
			else {
				MessageBox(hwnd, "No puede dejar campos vacíos. (Nombre del dueño)", "Aviso", MB_ICONEXCLAMATION);
				correcto = false;
			}

			length = GetWindowTextLength(hEdNombreM);
			if (length > 0) {
				GetWindowText(hEdNombreM, buffName, length+1);
				string s(buffName);
				if (verificarNum(s)) {
					MessageBox(hwnd, "Nombre de Mascota no deben contener números.", "Aviso", MB_ICONEXCLAMATION);
					correcto = false;
				}
				else {
					aux->nombreMascota = s;
				}
			}
			else {
				MessageBox(hwnd, "No puede dejar campos vacíos. (Nombre de la mascota)", "Aviso", MB_ICONEXCLAMATION);
				correcto = false;
			}

			length = GetWindowTextLength(hEdTel);
			if (length > 0) {
				if (length == 8 || length == 10) {
					GetWindowText(hEdTel, buff, length+1);
					string s(buff);
					aux->telefono = s;
				}
				else {
					MessageBox(hwnd, "El teléfono debe ser de 8 o 10 caracteres.", "Aviso", MB_ICONEXCLAMATION);
					correcto = false;
				}
			}
			else {
				MessageBox(hwnd, "No puede dejar campos vacíos. (Teléfono)", "Aviso", MB_ICONEXCLAMATION);
			}

			int indice = SendMessage(hCbEspecie, CB_GETCURSEL, 0, 0);
			if (indice == -1) {
				MessageBox(hwnd, "Seleccione la especie de su mascota.", "AVISO", MB_ICONEXCLAMATION);
				correcto = false;
			}
			else {
				length = GetWindowTextLength(hCbEspecie);
				GetWindowText(hCbEspecie, buff, length+1);
				string s(buff);
				aux->especie = s;
			}

			length = GetWindowTextLength(hTpFecha);
			GetWindowText(hTpFecha, buff, length+4);
			string f(buff);
			string Y = f.substr(6, 4);
			string m = f.substr(3, 2);
			string d = f.substr(0, 2);
			aux->fecha = f;

			GetWindowText(hTpHora, buff, 6);
			string h(buff);
			string I = h.substr(0, 2);
			aux->hora = h;

			length = GetWindowTextLength(hEdMotivo);
			if (length > 0) {
				GetWindowText(hEdMotivo, buff, length + 1);
				string m(buff);
				aux->motivoConsulta = m;
			}
			else {
				MessageBox(hwnd, "No puede dejar campos vacíos. (Motivo de consulta)", "Aviso", MB_ICONEXCLAMATION);
			}

			length = GetWindowTextLength(hEdCosto);
			if (length > 0) {
				GetWindowText(hEdCosto, buff, length + 1);
				string s(buff);
				if (verificarAlfa(s)) {
					MessageBox(hwnd, "El costo no debe contener letras.", "AVISO", MB_ICONEXCLAMATION);
					correcto = false;
				}
				else {
					aux->costo = stof(s);
				}
			}
			else {
				MessageBox(hwnd, "No puede dejar campos vacíos (Costo).", "Aviso", MB_ICONEXCLAMATION);
			}

			if (IsDlgButtonChecked(hwnd, RD_NC_CONTADO) == BST_CHECKED) {
				aux->formaPago = 1;
			}
			else if (IsDlgButtonChecked(hwnd, RD_NC_3MESES) == BST_CHECKED) {
				aux->formaPago = 3;
			}
			else if (IsDlgButtonChecked(hwnd, RD_NC_6MESES) == BST_CHECKED) {
				aux->formaPago = 6;
			}
			else if (IsDlgButtonChecked(hwnd, RD_NC_9MESES) == BST_CHECKED) {
				aux->formaPago = 9;
			}
			else {
				MessageBox(hwnd, "Seleccione una forma de pago.", "AVISO", MB_ICONEXCLAMATION);
			}

			if (correcto) {
				SendMessage(hLbAgenda, LB_ADDSTRING, 0, (LPARAM)buffName);
				MessageBox(hwnd, "Cita agregada.", "Mensaje", MB_OK);
				EndDialog(hwnd, 0);
			}
		}
	}break;
	case WM_TIMER: {
		HWND hEdNombreD = GetDlgItem(hwnd, EDT_NC_DNOMBRE);
		HWND hEdNombreM = GetDlgItem(hwnd, EDT_NC_MNOMBRE);
		HWND hEdTel = GetDlgItem(hwnd, EDT_NC_TELEFONO);
		HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
		HWND hTpFecha = GetDlgItem(hwnd, IDC_DATETIMEPICKER1);
		HWND hTpHora = GetDlgItem(hwnd, IDC_DATETIMEPICKER2);
		HWND hEdMotivo = GetDlgItem(hwnd, EDT_NC_MOTIVO);
		HWND hEdCosto = GetDlgItem(hwnd, EDT_NC_COSTO);

		int nombreD = GetWindowTextLength(hEdNombreD);
		int nombreM = GetWindowTextLength(hEdNombreM);
		int tel = GetWindowTextLength(hEdTel);
		int especie = GetWindowTextLength(hCbEspecie);
		int fecha = GetWindowTextLength(hTpFecha);
		int hora = GetWindowTextLength(hTpHora);
		int motivo = GetWindowTextLength(hEdMotivo);
		int costo = GetWindowTextLength(hEdCosto);


	}break;
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

bool verificarNum(string c) {
	bool r = false;
	int n = c.size();
	for (int i = 0; i < n; i++) {
		if (c[i] >= 48 && c[i] <= 57) {
			r = true;
			break;
		}
	}
	return r;
}

bool verificarAlfa(string c) {
	bool r = false;
	int n = c.size();
	for (int i = 0; i < n; i++) {
		if (c[i] == 47 && c[i] <= 45 && c[i] >= 58) {
			r = true;
			break;
		}
	}
	return r;
}