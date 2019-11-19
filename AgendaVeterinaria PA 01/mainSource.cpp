#include <Windows.h>
#include <string>
#include <time.h>
#include "resource.h"
using namespace std;

//msdn
//StackOverflow
//C SHEEL
//Videojuego TIS 100

#define TM_RELOJ 3000
#define TM_NC_RELOJ 3001
#define TM_EDC_RELOJ 3002

#pragma region VariablesGlobales
HINSTANCE hInstGlobal;
HMENU hBarraMenu;
HWND hAgenda;
HWND hNuevaCita;
HWND hEditarCita;
HWND hPagarCita;
HWND hEditarDoctor;

HWND hLbAgenda;
HWND hLblReloj;
HWND hLblNombreMedico;
HWND hLblCedula;

time_t allTime;
struct tm *tiempoActual;

struct CITA {
	CITA *prev;
	string nombreDue�o;
	string nombreMascota;
	string telefono;
	string especie;
	string motivoConsulta;
	string image[2];
	string fechaString;
	string horaString;
	int year;
	int month;
	int day;
	int hour;
	int minutes;
	int especieIndex;
	int formaPago;
	float costo;
	CITA *next;
}*origin, *aux;

char nombreMedico[50] = {NULL};
char cedula[20] = {NULL};
bool salida = false;
#pragma endregion

#pragma region PrototipoFunciones
bool verificarNum(string);
bool verificarAlfa(string);
void ordenamiento();
void impresion();
#pragma endregion

#pragma region PrototipoFunciones Ventana
BOOL CALLBACK agendaVentanaPrincipal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK nuevaCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK editarCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK editarInfoDoctor(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK pagarCita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK primerDoctor(HWND, UINT, WPARAM, LPARAM);
#pragma endregion

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	origin = aux = NULL;
	hInstGlobal = hInst;

	hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);

	ShowWindow(hAgenda, SW_SHOW);
	SetTimer(hAgenda, TM_RELOJ, 1000, NULL);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

#pragma region FuncionesVentanas
BOOL CALLBACK agendaVentanaPrincipal(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		hBarraMenu = GetMenu(hwnd);
		EnableMenuItem(hBarraMenu, BTN_AGENDA, MF_DISABLED);
		EnableMenuItem(hBarraMenu, BTN_NUEVACITA, MF_ENABLED);
		EnableMenuItem(hBarraMenu, BTN_EDITDOCINFO, MF_ENABLED);

		SendMessage(hLbAgenda, LB_RESETCONTENT, 0, 0);

		ordenamiento();

		hLbAgenda = GetDlgItem(hwnd, IDC_LISTACITAS);
		impresion();

		hLblReloj = GetDlgItem(hwnd, ST_MENU_RELOJ);

		hLblNombreMedico = GetDlgItem(hwnd, ST_MENU_DOCTOR);
		hLblCedula = GetDlgItem(hwnd, ST_MENU_CEDULA);
		if (nombreMedico[0] != NULL) {
			SetWindowText(hLblNombreMedico, nombreMedico);
			SetWindowText(hLblCedula, cedula);
		}
		else {
			int primerDoc = DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PRIMERDOCTOR), hwnd, primerDoctor);
		}
		
		HWND hLblListCount = GetDlgItem(hwnd, ST_LISTCOUNT);
		int lista = SendMessage(hLbAgenda, LB_GETCOUNT, 0, 0);
		string n = to_string(lista);
		char listaC[20];
		strcpy(listaC, n.c_str());
		SetWindowText(hLblListCount, listaC);
	}break;
	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			if (MessageBox(hwnd, "�Seguro que quiere salir del programa?", "SALIR", MB_YESNO) == IDYES) {
				salida = true;
				KillTimer(hwnd, TM_RELOJ);
				DestroyWindow(hAgenda);
			}
			break;
		}
		if (LOWORD(wParam) == BTN_EDITDOCINFO && HIWORD(wParam) == BN_CLICKED) {
			KillTimer(hAgenda, TM_RELOJ);
			DestroyWindow(hAgenda);
			hEditarDoctor = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_EDITDOCTOR), NULL, editarInfoDoctor);
			ShowWindow(hEditarDoctor, SW_SHOW);
		}
		if (LOWORD(wParam) == BTN_NUEVACITA && HIWORD(wParam) == BN_CLICKED) {
			aux = origin;
			KillTimer(hAgenda, TM_RELOJ);
			DestroyWindow(hAgenda);
			hNuevaCita = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_NUEVACITA), NULL, nuevaCita);
			ShowWindow(hNuevaCita, SW_SHOW);
			SetTimer(hNuevaCita, TM_NC_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_SELECT && HIWORD(wParam) == BN_CLICKED) {
			int index = SendMessage(hLbAgenda, LB_GETCURSEL, 0, 0);
			if (index > -1) {
				char buffer[80];
				SendMessage(hLbAgenda, LB_GETTEXT, index, (LPARAM)buffer);
				string cita(buffer);

				int n = cita.size();
				int toFirstNumStr = -1;
				for (int i = 0; i < n; i++) {
					toFirstNumStr++;
					if (cita[i] >= 48 && cita[i] <= 57) {
						break;
					}
				}

				string fechaLista = cita.substr(toFirstNumStr, 10);
				string horaLista = cita.substr(toFirstNumStr + 15, 14);

				aux = origin;
				while (aux != NULL) {
					if (fechaLista.compare(aux->fechaString) == 0) {
						if (horaLista.compare(aux->horaString) == 0) {
							break;
						}
					}
					aux = aux->next;
				}

				HWND hStMascota = GetDlgItem(hwnd, ST_INFO_MASCOTA);
				strcpy(buffer, aux->nombreMascota.c_str());
				SetWindowText(hStMascota, buffer);

				HWND hStEspecie = GetDlgItem(hwnd, ST_INFO_ESPECIE);
				strcpy(buffer, aux->especie.c_str());
				SetWindowText(hStEspecie, buffer);

				HWND hStDueno = GetDlgItem(hwnd, ST_INFO_DUENO);
				strcpy(buffer, aux->nombreDue�o.c_str());
				SetWindowText(hStDueno, buffer);

				HWND hStTelefono = GetDlgItem(hwnd, ST_INFO_TELEFONO);
				strcpy(buffer, aux->telefono.c_str());
				SetWindowText(hStTelefono, buffer);

				HWND hStFechaHora = GetDlgItem(hwnd, ST_INFO_FECHA);
				string infoFechaHora = aux->fechaString + " a las " + aux->horaString;
				strcpy(buffer, infoFechaHora.c_str());
				SetWindowText(hStFechaHora, buffer);

				HWND hStCosto = GetDlgItem(hwnd, ST_INFO_COSTO);
				char costoC[20];
				snprintf(costoC, sizeof(costoC), "%.2f", aux->costo);
				string costo(costoC);
				if (aux->formaPago == 3) {
					costo = "$" + costo + " a 3 meses sin intereses.";
				}
				else if (aux->formaPago == 6) {
					costo = "$" + costo + " a 6 meses sin intereses.";
				}
				else if (aux->formaPago == 9) {
					costo = "$" + costo + " a 9 meses sin intereses.";
				}
				else {
					costo = "$" + costo + " de contado.";
				}
				strcpy(buffer, costo.c_str());
				SetWindowText(hStCosto, buffer);

				HWND hStMotivo = GetDlgItem(hwnd, ST_INFO_MOTIVO);
				strcpy(buffer, aux->motivoConsulta.c_str());
				SetWindowText(hStMotivo, buffer);

				HWND hBtnEdit = GetDlgItem(hwnd, BTN_EDITARCITA);
				HWND hBtnPagar = GetDlgItem(hwnd, BTN_PAGARCITA);
				HWND hBtnEliminar = GetDlgItem(hwnd, BTN_ELIMINARCITA);
				EnableWindow(hBtnEdit, true);
				EnableWindow(hBtnPagar, true);
				EnableWindow(hBtnEliminar, true);
			}
		}
		if (LOWORD(wParam) == BTN_PAGARCITA && HIWORD(wParam) == BN_CLICKED) {
			DestroyWindow(hAgenda);
			hPagarCita = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_PAGOCITA), NULL, pagarCita);
			ShowWindow(hPagarCita, SW_SHOW);
		}
		if (LOWORD(wParam) == BTN_EDITARCITA && HIWORD(wParam) == BN_CLICKED) {
			DestroyWindow(hAgenda);
			hEditarCita = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_EDITARCITA), NULL, editarCita);
			ShowWindow(hEditarCita, SW_SHOW);
			SetTimer(hEditarCita, TM_EDC_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_ELIMINARCITA && HIWORD(wParam) == BN_CLICKED) {
			if (MessageBox(hwnd, "�Seguro que desea eliminar esta cita?", "Eliminar Cita", MB_OKCANCEL) == IDOK) {
				SendMessage(hLbAgenda, LB_RESETCONTENT, 0, 0);
			    if (aux->next == NULL && aux->prev == NULL) {
				delete aux;
				aux = origin = NULL;
			    }
				else if (aux->prev == NULL) {
					origin = origin->next;
					aux->next->prev = NULL;
					delete aux;
					aux = origin;
				}
				else if (aux->next == NULL) {
					aux->prev->next = NULL;
					delete aux;
					aux = origin;
				}
				else {
					aux->prev->next = aux->next;
					aux->next->prev = aux->prev;
					delete aux;
					aux = origin;
				}

				ordenamiento();
				impresion();

				HWND hLblListCount = GetDlgItem(hwnd, ST_LISTCOUNT);
				int lista = SendMessage(hLbAgenda, LB_GETCOUNT, 0, 0);
				char listaC[20];
				_itoa(lista, listaC, 10);
				SetWindowText(hLblListCount, listaC);

				HWND hBtnEdit = GetDlgItem(hwnd, BTN_EDITARCITA);
				HWND hBtnPagar = GetDlgItem(hwnd, BTN_PAGARCITA);
				HWND hBtnEliminar = GetDlgItem(hwnd, BTN_ELIMINARCITA);
				EnableWindow(hBtnEdit, false);
				EnableWindow(hBtnPagar, false);
				EnableWindow(hBtnEliminar, false);

				HWND hStMascota = GetDlgItem(hwnd, ST_INFO_MASCOTA);
				SetWindowText(hStMascota, "");
				HWND hStEspecie = GetDlgItem(hwnd, ST_INFO_ESPECIE);
				SetWindowText(hStEspecie, "");
				HWND hStDueno = GetDlgItem(hwnd, ST_INFO_DUENO);
				SetWindowText(hStDueno, "");
				HWND hStTelefono = GetDlgItem(hwnd, ST_INFO_TELEFONO);
				SetWindowText(hStTelefono, "");
				HWND hStFechaHora = GetDlgItem(hwnd, ST_INFO_FECHA);
				SetWindowText(hStFechaHora, "");
				HWND hStCosto = GetDlgItem(hwnd, ST_INFO_COSTO);
				SetWindowText(hStCosto, "");
				HWND hStMotivo = GetDlgItem(hwnd, ST_INFO_MOTIVO);
				SetWindowText(hStMotivo, "");
			}
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
		if (salida) {
			PostQuitMessage(0);
		}
	    break;
	}
	return FALSE;
}

BOOL CALLBACK nuevaCita(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
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

		hBarraMenu = GetMenu(hwnd);
		EnableMenuItem(hBarraMenu, BTN_NUEVACITA, MF_DISABLED);
		EnableMenuItem(hBarraMenu, BTN_AGENDA, MF_ENABLED);
		EnableMenuItem(hBarraMenu, BTN_EDITDOCINFO, MF_ENABLED);

		HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Perro");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Gato");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Loro");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Tortuga");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Pez");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Lagarto");

		HWND hLblNombreMedicoNC = GetDlgItem(hwnd, ST_NC_DOCTOR);
		HWND hLblCedulaNC = GetDlgItem(hwnd, ST_NC_CEDULA);
		SetWindowText(hLblNombreMedicoNC, nombreMedico);
		SetWindowText(hLblCedulaNC, cedula);

		hLblReloj = GetDlgItem(hwnd, ST_NC_RELOJ);
	}break;
	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			if (MessageBox(hwnd, "�Seguro que quiere salir del programa?", "SALIR", MB_YESNO) == IDYES) {
				if (aux->next == NULL && aux->prev == NULL) {
					delete aux;
					aux = origin = NULL;
				}
				else {
					aux->prev->next = NULL;
					delete aux;
					aux = origin;
				}

				salida = true;
				KillTimer(hwnd, TM_NC_RELOJ);
				DestroyWindow(hNuevaCita);
			}
		}
		if (LOWORD(wParam) == BTN_AGENDA && HIWORD(wParam) == BN_CLICKED) {
			if (aux->next == NULL && aux->prev == NULL) {
				delete aux;
				aux = origin = NULL;
			}
			else {
				aux->prev->next = NULL;
				delete aux;
				aux = origin;
			}

			KillTimer(hNuevaCita, TM_NC_RELOJ);
			DestroyWindow(hNuevaCita);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_EDITDOCINFO && HIWORD(wParam) == BN_CLICKED) {
			if (aux->next == NULL && aux->prev == NULL) {
				delete aux;
				aux = origin = NULL;
			}
			else {
				aux->prev->next = NULL;
				delete aux;
				aux = origin;
			}

			KillTimer(hNuevaCita, TM_NC_RELOJ);
			DestroyWindow(hNuevaCita);
			hEditarDoctor = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_EDITDOCTOR), NULL, editarInfoDoctor);
			ShowWindow(hEditarDoctor, SW_SHOW);
		}
		if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
			char buff[256];
			char buffName[50];
			int length;
			HWND hEdNombreD = GetDlgItem(hwnd, EDT_NC_DNOMBRE);
			length = GetWindowTextLength(hEdNombreD);
			if (length > 0) {
				GetWindowText(hEdNombreD, buff, length+1);
				string s(buff);
				if (verificarNum(s)) {
					MessageBox(hwnd, "El nombre de due�o no deben contener n�meros.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
				else {
					aux->nombreDue�o = s;
				}
			}
			else {
				MessageBox(hwnd, "Falta llenar el nombre del due�o.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdTel = GetDlgItem(hwnd, EDT_NC_TELEFONO);
			length = GetWindowTextLength(hEdTel);
			if (length > 0) {
				if (length == 8 || length == 10 || length == 12) {
					GetWindowText(hEdTel, buff, length + 1);
					string s(buff);
					aux->telefono = s;
				}
				else {
					MessageBox(hwnd, "El tel�fono debe ser de 8, 10 o 12 caracteres.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
			}
			else {
				MessageBox(hwnd, "Falta llenar el tel�fono.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdNombreM = GetDlgItem(hwnd, EDT_NC_MNOMBRE);
			length = GetWindowTextLength(hEdNombreM);
			if (length > 0) {
				GetWindowText(hEdNombreM, buffName, length+1);
				string s(buffName);
				if (verificarNum(s)) {
					MessageBox(hwnd, "El nombre de la mascota no deben contener n�meros.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
				else {
					aux->nombreMascota = s;
				}
			}
			else {
				MessageBox(hwnd, "Falta llenar el nombre de la mascota.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
			int indice = SendMessage(hCbEspecie, CB_GETCURSEL, 0, 0);
			if (indice == -1) {
				MessageBox(hwnd, "Seleccione la especie de su mascota.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}
			else {
				length = GetWindowTextLength(hCbEspecie);
				GetWindowText(hCbEspecie, buff, length+1);
				string s(buff);
				aux->especie = s;
				aux->especieIndex = indice;
			}

			HWND hTpFecha = GetDlgItem(hwnd, IDC_DATETIMEPICKER1);
			length = GetWindowTextLength(hTpFecha);
			GetWindowText(hTpFecha, buff, length+4);
			string f(buff);
			string Y = f.substr(6, 4);
			string m = f.substr(3, 2);
			string d = f.substr(0, 2);
			HWND hTpHora = GetDlgItem(hwnd, IDC_DATETIMEPICKER2);
			GetWindowText(hTpHora, buff, 15);
			string h(buff);
			string I = h.substr(0, 2);
			string min = h.substr(3, 2);
			string amPm = h.substr(9, 1);
			string stdHour;
			//VALIDCI�N DE FECHA NO ANTIGUA
			int yearComp = stoi(Y);
			int monthComp = stoi(m);
			int dayComp = stoi(d);
			int hourComp = stoi(I);
			int minuteComp = stoi(min);
			if (yearComp >= tiempoActual->tm_year + 1900) {
				if (yearComp == tiempoActual->tm_year + 1900) {
					if (monthComp >= tiempoActual->tm_mon + 1) {
						if (monthComp == tiempoActual->tm_mon + 1) {
							if (dayComp >= tiempoActual->tm_mday) {
								if (dayComp == tiempoActual->tm_mday) {
									if (amPm == "p") {
										if (hourComp != 12) {
											hourComp += 12;
										}
									}
									if (amPm == "a" && hourComp == 12) {
										hourComp -= 12;
									}
									if (hourComp >= tiempoActual->tm_hour) {
										if (hourComp == tiempoActual->tm_hour) {
											if (minuteComp > tiempoActual->tm_min) {
												aux->year = yearComp;
												aux->month = monthComp;
												aux->day = dayComp;
												aux->hour = hourComp;
												aux->minutes = minuteComp;
											}
											else {
												MessageBox(hwnd, "La hora no debe ser igual o pasada al tiempo actual.", "AVISO", MB_ICONEXCLAMATION);
												break;
											}
										}
										else {
											aux->year = yearComp;
											aux->month = monthComp;
											aux->day = dayComp;
											aux->hour = hourComp;
											aux->minutes = minuteComp;
										}
									}
									else {
										MessageBox(hwnd, "La hora no debe ser igual o pasada al tiempo actual.", "AVISO", MB_ICONEXCLAMATION);
										break;
									}
								}
								else {
									if (amPm == "p") {
										if (hourComp != 12) {
											hourComp += 12;
										}
									}
									if (amPm == "a" && hourComp == 12) {
										hourComp -= 12;
									}
									aux->year = yearComp;
									aux->month = monthComp;
									aux->day = dayComp;
									aux->hour = hourComp;
									aux->minutes = minuteComp;
								}
							}
							else {
								MessageBox(hwnd, "Coloque fecha v�lida, el d�a debe ser actual o pr�ximo.", "AVISO", MB_ICONEXCLAMATION);
								break;
							}
						}
						else {
							if (amPm == "p") {
								if (hourComp != 12) {
									hourComp += 12;
								}
							}
							if (amPm == "a" && hourComp == 12) {
								hourComp -= 12;
							}
							aux->year = yearComp;
							aux->month = monthComp;
							aux->day = dayComp;
							aux->hour = hourComp;
							aux->minutes = minuteComp;
						}
					}
					else {
						MessageBox(hwnd, "Coloque fecha v�lida, el mes debe ser actual o pr�ximo.", "AVISO", MB_ICONEXCLAMATION);
						break;
					}
				}
				else {
					if (amPm == "p") {
						if (hourComp != 12) {
							hourComp += 12;
						}
					}
					if (amPm == "a" && hourComp == 12) {
						hourComp -= 12;
					}
					aux->year = yearComp;
					aux->month = monthComp;
					aux->day = dayComp;
					aux->hour = hourComp;
					aux->minutes = minuteComp;
				}
			}
			else {
				MessageBox(hwnd, "Coloque fecha v�lida, el a�o debe ser actual o pr�ximo.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}
			//VALIDACI�N DE FECHA NO REPETIDA
			if (amPm == "p") {
				stdHour = I + ":" + min + " P.M.";
			}
			else {
				stdHour = I + ":" + min + " A.M.";
			}
			CITA *auxCF = origin;
			bool fechaRepetida = false;
			while (auxCF != NULL) {
				if (f.compare(auxCF->fechaString) == 0) {
					if (stdHour.compare(auxCF->horaString) == 0) {
						fechaRepetida = true;
						break;
					}
				}
				auxCF = auxCF->next;
			}
			if (fechaRepetida) {
				MessageBox(hwnd, "La fecha y hora de la cita ya no est� disponible. Ya existe otra cita en este horario.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}
			else {
				aux->fechaString = f;
				aux->horaString = stdHour;
			}

			HWND hEdMotivo = GetDlgItem(hwnd, EDT_NC_MOTIVO);
			length = GetWindowTextLength(hEdMotivo);
			if (length > 0) {
				GetWindowText(hEdMotivo, buff, length + 1);
				string m(buff);
				aux->motivoConsulta = m;
			}
			else {
				MessageBox(hwnd, "Especifique el motivo de la consulta.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdCosto = GetDlgItem(hwnd, EDT_NC_COSTO);
			length = GetWindowTextLength(hEdCosto);
			if (length > 0) {
				GetWindowText(hEdCosto, buff, length + 1);
				string s(buff);
				if (verificarAlfa(s)) {
					MessageBox(hwnd, "El costo no debe contener letras o m�s de un punto decimal. Corrija.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
				else {
					aux->costo = stof(s);
				}
			}
			else {
				MessageBox(hwnd, "Agregue el costo de la consulta.", "Aviso", MB_ICONEXCLAMATION);
				break;
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
				break;
			}
			MessageBox(hwnd, "Cita agregada.", "DATOS CORRECTOS", MB_OK);
			
			aux = origin;

			KillTimer(hNuevaCita, TM_NC_RELOJ);
			DestroyWindow(hNuevaCita);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
	}break;
	case WM_TIMER: {
		time(&allTime);
		tiempoActual = localtime(&allTime);
		char reloj[80];
		strftime(reloj, 80, "Hoy es: %d/%m/%Y  %I:%M:%S", tiempoActual);
		SetWindowText(hLblReloj, reloj);
	}break;
	case WM_DESTROY:
		if (salida) {
			PostQuitMessage(0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK editarInfoDoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		hBarraMenu = GetMenu(hwnd);
		EnableMenuItem(hBarraMenu, BTN_EDITDOCINFO, MF_DISABLED);
		EnableMenuItem(hBarraMenu, BTN_AGENDA, MF_ENABLED);
		EnableMenuItem(hBarraMenu, BTN_NUEVACITA, MF_ENABLED);

		HWND hEdNombreMedico = GetDlgItem(hwnd, EDT_EDM_DOCTOR);
		HWND hEdCedula = GetDlgItem(hwnd, EDT_EDM_CEDULA);
		if (nombreMedico == "") {
			SetWindowText(hEdNombreMedico, "");
			SetWindowText(hEdCedula, "");
		}
		else {
			SetWindowText(hEdNombreMedico, nombreMedico);
			SetWindowText(hEdCedula, cedula);
		}
	}break;
	case WM_COMMAND:
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			if (MessageBox(hwnd, "�Seguro que quiere salir del programa?", "SALIR", MB_YESNO) == IDYES) {
				salida = true;
				DestroyWindow(hEditarDoctor);
			}
			break;
		}
		if (LOWORD(wParam) == BTN_AGENDA && HIWORD(wParam) == BN_CLICKED) {
			DestroyWindow(hEditarDoctor);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_NUEVACITA && HIWORD(wParam) == BN_CLICKED) {
			DestroyWindow(hEditarDoctor);
			hNuevaCita = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_NUEVACITA), NULL, nuevaCita);
			ShowWindow(hNuevaCita, SW_SHOW);
			SetTimer(hNuevaCita, TM_NC_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == ID_EDM_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			DestroyWindow(hEditarDoctor);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
			break;
		}
		if (LOWORD(wParam = ID_EDM_OK && HIWORD(wParam) == BN_CLICKED)) {
			HWND hEdNombreMedico = GetDlgItem(hwnd, EDT_EDM_DOCTOR);
			int length = GetWindowTextLength(hEdNombreMedico);
			if (length > 0) {
				GetWindowText(hEdNombreMedico, nombreMedico, length + 1);
				string m(nombreMedico);
				if (verificarNum(m)) {
					MessageBox(hwnd, "El nombre del m�dico no debe contener n�meros.", "AVISO", MB_ICONEXCLAMATION);
					break;
				} 
			}
			else {
				MessageBox(hwnd, "Llene el nombre del m�dico.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdCedula = GetDlgItem(hwnd, EDT_EDM_CEDULA);
			length = GetWindowTextLength(hEdCedula);
			if (length > 0) {
				GetWindowText(hEdCedula, cedula, length + 1);
			}
			else {
				MessageBox(hwnd, "Llene la c�dula.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			DestroyWindow(hEditarDoctor);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		break;
	case WM_DESTROY:
		if (salida) {
			PostQuitMessage(0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK pagarCita(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		hBarraMenu = GetMenu(hwnd);
		EnableMenuItem(hBarraMenu, BTN_AGENDA, MF_ENABLED);
		EnableMenuItem(hBarraMenu, BTN_NUEVACITA, MF_ENABLED);
		EnableMenuItem(hBarraMenu, BTN_EDITDOCINFO, MF_ENABLED);

		HWND hLblNombreMedicoPC = GetDlgItem(hwnd, ST_PC_DOCTOR);
		HWND hLblCedulaPC = GetDlgItem(hwnd, ST_PC_CEDULA);
		SetWindowText(hLblNombreMedicoPC, nombreMedico);
		SetWindowText(hLblCedulaPC, cedula);

		char buffer[80];
		HWND hStMascota = GetDlgItem(hwnd, ST_PC_MNOMBRE);
		strcpy(buffer, aux->nombreMascota.c_str());
		SetWindowText(hStMascota, buffer);

		HWND hStEspecie = GetDlgItem(hwnd, ST_PC_ESPECIE);
		strcpy(buffer, aux->especie.c_str());
		SetWindowText(hStEspecie, buffer);

		HWND hStDueno = GetDlgItem(hwnd, ST_PC_DNOMBRE);
		strcpy(buffer, aux->nombreDue�o.c_str());
		SetWindowText(hStDueno, buffer);

		HWND hStTelefono = GetDlgItem(hwnd, ST_PC_TELEFONO);
		strcpy(buffer, aux->telefono.c_str());
		SetWindowText(hStTelefono, buffer);

		HWND hStFechaHora = GetDlgItem(hwnd, ST_PC_FECHAHORA);
		string infoFechaHora = aux->fechaString + " a las " + aux->horaString;
		strcpy(buffer, infoFechaHora.c_str());
		SetWindowText(hStFechaHora, buffer);

		HWND hStMotivo = GetDlgItem(hwnd, EDT_PC_MOTIVO);
		strcpy(buffer, aux->motivoConsulta.c_str());
		SetWindowText(hStMotivo, buffer);

		HWND hStCosto = GetDlgItem(hwnd, ST_PC_COSTO);
		char costoC[20];
		snprintf(costoC, sizeof(costoC), "%.2f", aux->costo);
		string costo(costoC);
		if (aux->formaPago == 3) {
			costo = costo + " a 3 meses sin intereses.";
		}
		else if (aux->formaPago == 6) {
			costo = costo + " a 6 meses sin intereses.";
		}
		else if (aux->formaPago == 9) {
			costo = costo + " a 9 meses sin intereses.";
		}
		else {
			costo = costo + " de contado.";
		}
		strcpy(buffer, costo.c_str());
		SetWindowText(hStCosto, buffer);

		float iva = aux->costo * .12;
		float precioTot = aux->costo + iva;
		float precioMeses = precioTot / aux->formaPago;

		HWND hStIva = GetDlgItem(hwnd, ST_IVA);
		char ivaC[20];
		snprintf(ivaC, sizeof(ivaC), "%.2f", iva);
		SetWindowText(hStIva, ivaC);

		HWND hStPrecioTot = GetDlgItem(hwnd, ST_PRECIOTOT);
		char precioTotC[20];
		snprintf(precioTotC, sizeof(precioTotC), "%.2f", precioTot);
		SetWindowText(hStPrecioTot, precioTotC);

		HWND hStPrecioMes = GetDlgItem(hwnd, ST_PORMES);
		if (aux->formaPago != 1) {
			char precioMesC[20];
			snprintf(precioMesC, sizeof(precioMesC), "%.2f", precioMeses);
			SetWindowText(hStPrecioMes, precioMesC);
		}
		else {
			SetWindowText(hStPrecioMes, precioTotC);
		}
	}break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_PC_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			aux = origin;
			DestroyWindow(hPagarCita);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
			if (aux->next == NULL && aux->prev == NULL) {
				delete aux;
				aux = origin = NULL;
			}
			else if (aux->prev == NULL) {
				origin = origin->next;
				aux->next->prev = NULL;
				delete aux;
				aux = origin;
			}
			else if (aux->next == NULL) {
				aux->prev->next = NULL;
				delete aux;
				aux = origin;
			}
			else {
				aux->prev->next = aux->next;
				aux->next->prev = aux->prev;
				delete aux;
				aux = origin;
			}

			MessageBox(hwnd, "Se ha pagado la cita. Tenga un buen d�a.", "PAGADO", MB_OK);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			DestroyWindow(hPagarCita);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			if (MessageBox(hwnd, "�Seguro que quiere salir del programa?", "SALIR", MB_YESNO) == IDYES) {
				aux = origin;
				salida = true;
				DestroyWindow(hPagarCita);
			}
			break;
		}
		if (LOWORD(wParam) == BTN_AGENDA && HIWORD(wParam) == BN_CLICKED) {
			aux = origin;
			DestroyWindow(hPagarCita);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_NUEVACITA && HIWORD(wParam) == BN_CLICKED) {
			aux = origin;
			DestroyWindow(hPagarCita);
			hNuevaCita = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_NUEVACITA), NULL, nuevaCita);
			ShowWindow(hNuevaCita, SW_SHOW);
			SetTimer(hNuevaCita, TM_NC_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_EDITDOCINFO && HIWORD(wParam) == BN_CLICKED) {
			aux = origin;
			DestroyWindow(hPagarCita);
			hEditarDoctor = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_EDITDOCTOR), NULL, editarInfoDoctor);
			ShowWindow(hEditarDoctor, SW_SHOW);
		}
		break;
	case WM_DESTROY:
		if (salida) {
			PostQuitMessage(0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK editarCita(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		hBarraMenu = GetMenu(hwnd);
		EnableMenuItem(hBarraMenu, BTN_NUEVACITA, MF_DISABLED);
		EnableMenuItem(hBarraMenu, BTN_AGENDA, MF_DISABLED);
		EnableMenuItem(hBarraMenu, BTN_EDITDOCINFO, MF_DISABLED);

		HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Perro");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Gato");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Loro");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Tortuga");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Pez");
		SendMessage(hCbEspecie, CB_ADDSTRING, 0, (LPARAM)"Lagarto");

		HWND hLblNombreMedicoNC = GetDlgItem(hwnd, ST_NC_DOCTOR);
		HWND hLblCedulaNC = GetDlgItem(hwnd, ST_NC_CEDULA);
		SetWindowText(hLblNombreMedicoNC, nombreMedico);
		SetWindowText(hLblCedulaNC, cedula);

		hLblReloj = GetDlgItem(hwnd, ST_NC_RELOJ);

		char buffer[80];
		HWND hEdNombreD = GetDlgItem(hwnd, EDT_NC_DNOMBRE);
		strcpy(buffer, aux->nombreDue�o.c_str());
		SetWindowText(hEdNombreD, buffer);

		HWND hEdTelefono = GetDlgItem(hwnd, EDT_NC_TELEFONO);
		strcpy(buffer, aux->telefono.c_str());
		SetWindowText(hEdTelefono, buffer);

		HWND hEdNombreM = GetDlgItem(hwnd, EDT_NC_MNOMBRE);
		strcpy(buffer, aux->nombreMascota.c_str());
		SetWindowText(hEdNombreM, buffer);

		SendMessage(hCbEspecie, CB_SETCURSEL, aux->especieIndex, 0);

		HWND hStFechaA = GetDlgItem(hwnd, ST_NC_FECHAACTUAL);
		string fechaA = "Fecha de la cita: " + aux->fechaString + " a las " + aux->horaString;
		strcpy(buffer, fechaA.c_str());
		SetWindowText(hStFechaA, buffer);

		HWND hEdMotivo = GetDlgItem(hwnd, EDT_NC_MOTIVO);
		strcpy(buffer, aux->motivoConsulta.c_str());
		SetWindowText(hEdMotivo, buffer);

		HWND hEdCosto = GetDlgItem(hwnd, EDT_NC_COSTO);
		snprintf(buffer, sizeof(buffer), "%.2f", aux->costo);
		SetWindowText(hEdCosto, buffer);

		if (aux->formaPago == 3) {
			HWND hRdMeses = GetDlgItem(hwnd, RD_NC_3MESES);
			SendMessage(hRdMeses, BM_SETCHECK, BST_CHECKED, 1);
		}
		else if (aux->formaPago == 6) {
			HWND hRdMeses = GetDlgItem(hwnd, RD_NC_6MESES);
			SendMessage(hRdMeses, BM_SETCHECK, BST_CHECKED, 1);
		}
		else if (aux->formaPago == 9) {
			HWND hRdMeses = GetDlgItem(hwnd, RD_NC_9MESES);
			SendMessage(hRdMeses, BM_SETCHECK, BST_CHECKED, 1);
		}
		else {
			HWND hRdMeses = GetDlgItem(hwnd, RD_NC_CONTADO);
			SendMessage(hRdMeses, BM_SETCHECK, BST_CHECKED, 1);
		}
	}break;
	case WM_COMMAND:
		if (LOWORD(wParam) == BTN_EC_CANCELA && HIWORD(wParam) == BN_CLICKED) {
			aux = origin;
			KillTimer(hwnd, TM_EDC_RELOJ);
			DestroyWindow(hEditarCita);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
			char buff[256];
			char buffName[50];
			int length;
			HWND hEdNombreD = GetDlgItem(hwnd, EDT_NC_DNOMBRE);
			length = GetWindowTextLength(hEdNombreD);
			if (length > 0) {
				GetWindowText(hEdNombreD, buff, length + 1);
				string s(buff);
				if (verificarNum(s)) {
					MessageBox(hwnd, "El nombre de due�o no deben contener n�meros.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
				else {
					aux->nombreDue�o = s;
				}
			}
			else {
				MessageBox(hwnd, "Falta llenar el nombre del due�o.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdTel = GetDlgItem(hwnd, EDT_NC_TELEFONO);
			length = GetWindowTextLength(hEdTel);
			if (length > 0) {
				if (length == 8 || length == 10 || length == 12) {
					GetWindowText(hEdTel, buff, length + 1);
					string s(buff);
					aux->telefono = s;
				}
				else {
					MessageBox(hwnd, "El tel�fono debe ser de 8, 10 o 12 caracteres.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
			}
			else {
				MessageBox(hwnd, "Falta llenar el tel�fono.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdNombreM = GetDlgItem(hwnd, EDT_NC_MNOMBRE);
			length = GetWindowTextLength(hEdNombreM);
			if (length > 0) {
				GetWindowText(hEdNombreM, buffName, length + 1);
				string s(buffName);
				if (verificarNum(s)) {
					MessageBox(hwnd, "El nombre de la mascota no deben contener n�meros.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
				else {
					aux->nombreMascota = s;
				}
			}
			else {
				MessageBox(hwnd, "Falta llenar el nombre de la mascota.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hCbEspecie = GetDlgItem(hwnd, CB_NC_ESPECIE);
			int indice = SendMessage(hCbEspecie, CB_GETCURSEL, 0, 0);
			if (indice == -1) {
				MessageBox(hwnd, "Seleccione la especie de su mascota.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}
			else {
				length = GetWindowTextLength(hCbEspecie);
				GetWindowText(hCbEspecie, buff, length + 1);
				string s(buff);
				aux->especie = s;
				aux->especieIndex = indice;
			}

			HWND hTpFecha = GetDlgItem(hwnd, IDC_DATETIMEPICKER1);
			length = GetWindowTextLength(hTpFecha);
			GetWindowText(hTpFecha, buff, length + 4);
			string f(buff);
			string Y = f.substr(6, 4);
			string m = f.substr(3, 2);
			string d = f.substr(0, 2);
			HWND hTpHora = GetDlgItem(hwnd, IDC_DATETIMEPICKER2);
			GetWindowText(hTpHora, buff, 15);
			string h(buff);
			string I = h.substr(0, 2);
			string min = h.substr(3, 2);
			string amPm = h.substr(9, 1);
			string stdHour;
			//VALIDCI�N DE FECHA NO ANTIGUA
			int yearComp = stoi(Y);
			int monthComp = stoi(m);
			int dayComp = stoi(d);
			int hourComp = stoi(I);
			int minuteComp = stoi(min);
			if (yearComp >= tiempoActual->tm_year + 1900) {
				if (yearComp == tiempoActual->tm_year + 1900) {
					if (monthComp >= tiempoActual->tm_mon + 1) {
						if (monthComp == tiempoActual->tm_mon + 1) {
							if (dayComp >= tiempoActual->tm_mday) {
								if (dayComp == tiempoActual->tm_mday) {
									if (amPm == "p") {
										if (hourComp != 12) {
											hourComp += 12;
										}
									}
									if (amPm == "a" && hourComp == 12) {
										hourComp -= 12;
									}
									if (hourComp >= tiempoActual->tm_hour) {
										if (hourComp == tiempoActual->tm_hour) {
											if (minuteComp > tiempoActual->tm_min) {
												aux->year = yearComp;
												aux->month = monthComp;
												aux->day = dayComp;
												aux->hour = hourComp;
												aux->minutes = minuteComp;
												aux->fechaString = "";
												aux->horaString = "";
											}
											else {
												MessageBox(hwnd, "La hora no debe ser igual o pasada al tiempo actual.", "AVISO", MB_ICONEXCLAMATION);
												break;
											}
										}
										else {
											aux->year = yearComp;
											aux->month = monthComp;
											aux->day = dayComp;
											aux->hour = hourComp;
											aux->minutes = minuteComp;
											aux->fechaString = "";
											aux->horaString = "";
										}
									}
									else {
										MessageBox(hwnd, "La hora no debe ser igual o pasada al tiempo actual.", "AVISO", MB_ICONEXCLAMATION);
										break;
									}
								}
								else {
									if (amPm == "p") {
										if (hourComp != 12) {
											hourComp += 12;
										}
									}
									if (amPm == "a" && hourComp == 12) {
										hourComp -= 12;
									}
									aux->year = yearComp;
									aux->month = monthComp;
									aux->day = dayComp;
									aux->hour = hourComp;
									aux->minutes = minuteComp;
									aux->fechaString = "";
									aux->horaString = "";
								}
							}
							else {
								MessageBox(hwnd, "Coloque fecha v�lida, el d�a debe ser actual o pr�ximo.", "AVISO", MB_ICONEXCLAMATION);
								break;
							}
						}
						else {
							if (amPm == "p") {
								if (hourComp != 12) {
									hourComp += 12;
								}
							}
							if (amPm == "a" && hourComp == 12) {
								hourComp -= 12;
							}
							aux->year = yearComp;
							aux->month = monthComp;
							aux->day = dayComp;
							aux->hour = hourComp;
							aux->minutes = minuteComp;
							aux->fechaString = "";
							aux->horaString = "";
						}
					}
					else {
						MessageBox(hwnd, "Coloque fecha v�lida, el mes debe ser actual o pr�ximo.", "AVISO", MB_ICONEXCLAMATION);
						break;
					}
				}
				else {
					if (amPm == "p") {
						if (hourComp != 12) {
							hourComp += 12;
						}
					}
					if (amPm == "a" && hourComp == 12) {
						hourComp -= 12;
					}
					aux->year = yearComp;
					aux->month = monthComp;
					aux->day = dayComp;
					aux->hour = hourComp;
					aux->minutes = minuteComp;
					aux->fechaString = "";
					aux->horaString = "";
				}
			}
			else {
				MessageBox(hwnd, "Coloque fecha v�lida, el a�o debe ser actual o pr�ximo.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}
			//VALIDACI�N DE FECHA NO REPETIDA
			if (amPm == "p") {
				stdHour = I + ":" + min + " P.M.";
			}
			else {
				stdHour = I + ":" + min + " A.M.";
			}
			CITA *auxCF = origin;
			bool fechaRepetida = false;
			while (auxCF != NULL) {
				if (f.compare(auxCF->fechaString) == 0) {
					if (stdHour.compare(auxCF->horaString) == 0) {
						fechaRepetida = true;
						break;
					}
				}
				auxCF = auxCF->next;
			}
			if (fechaRepetida) {
				MessageBox(hwnd, "La fecha y hora de la cita ya no est� disponible. Ya existe otra cita en este horario.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}
			else {
				aux->fechaString = f;
				aux->horaString = stdHour;
			}

			HWND hEdMotivo = GetDlgItem(hwnd, EDT_NC_MOTIVO);
			length = GetWindowTextLength(hEdMotivo);
			if (length > 0) {
				GetWindowText(hEdMotivo, buff, length + 1);
				string m(buff);
				aux->motivoConsulta = m;
			}
			else {
				MessageBox(hwnd, "Especifique el motivo de la consulta.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdCosto = GetDlgItem(hwnd, EDT_NC_COSTO);
			length = GetWindowTextLength(hEdCosto);
			if (length > 0) {
				GetWindowText(hEdCosto, buff, length + 1);
				string s(buff);
				if (verificarAlfa(s)) {
					MessageBox(hwnd, "El costo no debe contener letras o m�s de un punto decimal. Corrija.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
				else {
					aux->costo = stof(s);
				}
			}
			else {
				MessageBox(hwnd, "Agregue el costo de la consulta.", "Aviso", MB_ICONEXCLAMATION);
				break;
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
				break;
			}
			MessageBox(hwnd, "Cita Modificada.", "DATOS CORRECTOS", MB_OK);

			aux = origin;

			KillTimer(hEditarCita, TM_EDC_RELOJ);
			DestroyWindow(hEditarCita);
			hAgenda = CreateDialog(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDA), NULL, agendaVentanaPrincipal);
			ShowWindow(hAgenda, SW_SHOW);
			SetTimer(hAgenda, TM_RELOJ, 1000, NULL);
		}
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) {
			if (MessageBox(hwnd, "�Seguro que quiere salir del programa?", "SALIR", MB_YESNO) == IDYES) {
				salida = true;
				KillTimer(hwnd, TM_EDC_RELOJ);
				DestroyWindow(hEditarCita);
			}
			break;
		}
		break;
	case WM_TIMER: {
		time(&allTime);
		tiempoActual = localtime(&allTime);
		char reloj[80];
		strftime(reloj, 80, "Hoy es: %d/%m/%Y  %I:%M:%S", tiempoActual);
		SetWindowText(hLblReloj, reloj);
	}break;
	case WM_DESTROY:
		if (salida) {
			PostQuitMessage(0);
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK primerDoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		MessageBox(hwnd, "Hola, doctor. Ingrese sus datos para empezar su agenda.", "BIENVENIDO", MB_ICONINFORMATION);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam = ID_PM_OK && HIWORD(wParam) == BN_CLICKED)) {
			HWND hEdNombreMedico = GetDlgItem(hwnd, EDT_EDM_DOCTOR);
			int length = GetWindowTextLength(hEdNombreMedico);
			if (length > 0) {
				GetWindowText(hEdNombreMedico, nombreMedico, length + 1);
				string m(nombreMedico);
				if (verificarNum(m)) {
					MessageBox(hwnd, "El nombre del m�dico no debe contener n�meros.", "AVISO", MB_ICONEXCLAMATION);
					break;
				}
			}
			else {
				MessageBox(hwnd, "Llene el nombre del m�dico.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			HWND hEdCedula = GetDlgItem(hwnd, EDT_EDM_CEDULA);
			length = GetWindowTextLength(hEdCedula);
			if (length > 0) {
				GetWindowText(hEdCedula, cedula, length + 1);
			}
			else {
				MessageBox(hwnd, "Llene la c�dula.", "AVISO", MB_ICONEXCLAMATION);
				break;
			}

			SetWindowText(hLblNombreMedico, nombreMedico);
			SetWindowText(hLblCedula, cedula);

			EndDialog(hwnd, 0);
		}
		break;
	}
	return FALSE;
}
#pragma endregion

#pragma region Funciones
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
	int dots = 0;
	for (int i = 0; i < n; i++) {
		if (c[i] < 47 && c[i] > 58) {
			r = true;
			break;
		}
		if (c[i] == 46) {
			dots++;
			if (dots == 2) {
				r = true;
				break;
			}
		}
	}
	return r;
}

void ordenamiento() {
	CITA *auxActual, *auxProx;
	auxActual = origin;
	while (auxActual != NULL) {
		auxProx = auxActual->next;
		while (auxProx != NULL) {
			if (auxActual->year > auxProx->year) {
				string tNombreD = auxProx->nombreDue�o;
				string tNombreM = auxProx->nombreMascota;
				string tTel = auxProx->telefono;
				string tEsp = auxProx->especie;
				string tMotivo = auxProx->motivoConsulta;
				string tFechaStr = auxProx->fechaString;
				string tHoraStr = auxProx->horaString;
				int tY = auxProx->year;
				int tM = auxProx->month;
				int tD = auxProx->day;
				int tH = auxProx->hour;
				int tMin = auxProx->minutes;
				int tEspInd = auxProx->especieIndex;
				int tPago = auxProx->formaPago;
				float tCosto = auxProx->costo;

				auxProx->nombreDue�o = auxActual->nombreDue�o;
				auxProx->nombreMascota = auxActual->nombreMascota;
				auxProx->telefono = auxActual->telefono;
				auxProx->especie = auxActual->especie;
				auxProx->motivoConsulta = auxActual->motivoConsulta;
				auxProx->fechaString = auxActual->fechaString;
				auxProx->horaString = auxActual->horaString;
				auxProx->year = auxActual->year;
				auxProx->month = auxActual->month;
				auxProx->day = auxActual->day;
				auxProx->hour = auxActual->hour;
				auxProx->minutes = auxActual->minutes;
				auxProx->especieIndex = auxActual->especieIndex;
				auxProx->formaPago = auxActual->formaPago;
				auxProx->costo = auxActual->costo;

				auxActual->nombreDue�o = tNombreD;
				auxActual->nombreMascota = tNombreM;
				auxActual->telefono = tTel;
				auxActual->especie = tEsp;
				auxActual->motivoConsulta = tMotivo;
				auxActual->fechaString = tFechaStr;
				auxActual->horaString = tHoraStr;
				auxActual->year = tY;
				auxActual->month = tM;
				auxActual->day = tD;
				auxActual->hour = tH;
				auxActual->minutes = tMin;
				auxActual->especieIndex = tEspInd;
				auxActual->formaPago = tPago;
				auxActual->costo = tCosto;
			}
			if (auxActual->year == auxProx->year && auxActual->month > auxProx->month) {
				string tNombreD = auxProx->nombreDue�o;
				string tNombreM = auxProx->nombreMascota;
				string tTel = auxProx->telefono;
				string tEsp = auxProx->especie;
				string tMotivo = auxProx->motivoConsulta;
				string tFechaStr = auxProx->fechaString;
				string tHoraStr = auxProx->horaString;
				int tY = auxProx->year;
				int tM = auxProx->month;
				int tD = auxProx->day;
				int tH = auxProx->hour;
				int tMin = auxProx->minutes;
				int tEspInd = auxProx->especieIndex;
				int tPago = auxProx->formaPago;
				float tCosto = auxProx->costo;

				auxProx->nombreDue�o = auxActual->nombreDue�o;
				auxProx->nombreMascota = auxActual->nombreMascota;
				auxProx->telefono = auxActual->telefono;
				auxProx->especie = auxActual->especie;
				auxProx->motivoConsulta = auxActual->motivoConsulta;
				auxProx->fechaString = auxActual->fechaString;
				auxProx->horaString = auxActual->horaString;
				auxProx->year = auxActual->year;
				auxProx->month = auxActual->month;
				auxProx->day = auxActual->day;
				auxProx->hour = auxActual->hour;
				auxProx->minutes = auxActual->minutes;
				auxProx->especieIndex = auxActual->especieIndex;
				auxProx->formaPago = auxActual->formaPago;
				auxProx->costo = auxActual->costo;

				auxActual->nombreDue�o = tNombreD;
				auxActual->nombreMascota = tNombreM;
				auxActual->telefono = tTel;
				auxActual->especie = tEsp;
				auxActual->motivoConsulta = tMotivo;
				auxActual->fechaString = tFechaStr;
				auxActual->horaString = tHoraStr;
				auxActual->year = tY;
				auxActual->month = tM;
				auxActual->day = tD;
				auxActual->hour = tH;
				auxActual->minutes = tMin;
				auxActual->especieIndex = tEspInd;
				auxActual->formaPago = tPago;
				auxActual->costo = tCosto;
			}
			if (auxActual->year == auxProx->year && auxActual->month == auxProx->month && auxActual->day > auxProx->day) {
				string tNombreD = auxProx->nombreDue�o;
				string tNombreM = auxProx->nombreMascota;
				string tTel = auxProx->telefono;
				string tEsp = auxProx->especie;
				string tMotivo = auxProx->motivoConsulta;
				string tFechaStr = auxProx->fechaString;
				string tHoraStr = auxProx->horaString;
				int tY = auxProx->year;
				int tM = auxProx->month;
				int tD = auxProx->day;
				int tH = auxProx->hour;
				int tMin = auxProx->minutes;
				int tEspInd = auxProx->especieIndex;
				int tPago = auxProx->formaPago;
				float tCosto = auxProx->costo;

				auxProx->nombreDue�o = auxActual->nombreDue�o;
				auxProx->nombreMascota = auxActual->nombreMascota;
				auxProx->telefono = auxActual->telefono;
				auxProx->especie = auxActual->especie;
				auxProx->motivoConsulta = auxActual->motivoConsulta;
				auxProx->fechaString = auxActual->fechaString;
				auxProx->horaString = auxActual->horaString;
				auxProx->year = auxActual->year;
				auxProx->month = auxActual->month;
				auxProx->day = auxActual->day;
				auxProx->hour = auxActual->hour;
				auxProx->minutes = auxActual->minutes;
				auxProx->especieIndex = auxActual->especieIndex;
				auxProx->formaPago = auxActual->formaPago;
				auxProx->costo = auxActual->costo;

				auxActual->nombreDue�o = tNombreD;
				auxActual->nombreMascota = tNombreM;
				auxActual->telefono = tTel;
				auxActual->especie = tEsp;
				auxActual->motivoConsulta = tMotivo;
				auxActual->fechaString = tFechaStr;
				auxActual->horaString = tHoraStr;
				auxActual->year = tY;
				auxActual->month = tM;
				auxActual->day = tD;
				auxActual->hour = tH;
				auxActual->minutes = tMin;
				auxActual->especieIndex = tEspInd;
				auxActual->formaPago = tPago;
				auxActual->costo = tCosto;
			}
			if (auxActual->year == auxProx->year && auxActual->month == auxProx->month && auxActual->day == auxProx->day && auxActual->hour > auxProx->hour) {
				string tNombreD = auxProx->nombreDue�o;
				string tNombreM = auxProx->nombreMascota;
				string tTel = auxProx->telefono;
				string tEsp = auxProx->especie;
				string tMotivo = auxProx->motivoConsulta;
				string tFechaStr = auxProx->fechaString;
				string tHoraStr = auxProx->horaString;
				int tY = auxProx->year;
				int tM = auxProx->month;
				int tD = auxProx->day;
				int tH = auxProx->hour;
				int tMin = auxProx->minutes;
				int tEspInd = auxProx->especieIndex;
				int tPago = auxProx->formaPago;
				float tCosto = auxProx->costo;

				auxProx->nombreDue�o = auxActual->nombreDue�o;
				auxProx->nombreMascota = auxActual->nombreMascota;
				auxProx->telefono = auxActual->telefono;
				auxProx->especie = auxActual->especie;
				auxProx->motivoConsulta = auxActual->motivoConsulta;
				auxProx->fechaString = auxActual->fechaString;
				auxProx->horaString = auxActual->horaString;
				auxProx->year = auxActual->year;
				auxProx->month = auxActual->month;
				auxProx->day = auxActual->day;
				auxProx->hour = auxActual->hour;
				auxProx->minutes = auxActual->minutes;
				auxProx->especieIndex = auxActual->especieIndex;
				auxProx->formaPago = auxActual->formaPago;
				auxProx->costo = auxActual->costo;

				auxActual->nombreDue�o = tNombreD;
				auxActual->nombreMascota = tNombreM;
				auxActual->telefono = tTel;
				auxActual->especie = tEsp;
				auxActual->motivoConsulta = tMotivo;
				auxActual->fechaString = tFechaStr;
				auxActual->horaString = tHoraStr;
				auxActual->year = tY;
				auxActual->month = tM;
				auxActual->day = tD;
				auxActual->hour = tH;
				auxActual->minutes = tMin;
				auxActual->especieIndex = tEspInd;
				auxActual->formaPago = tPago;
				auxActual->costo = tCosto;
			}
			if (auxActual->year == auxProx->year && auxActual->month == auxProx->month && auxActual->day == auxProx->day && auxActual->hour == auxProx->hour && auxActual->minutes > auxProx->minutes) {
				string tNombreD = auxProx->nombreDue�o;
				string tNombreM = auxProx->nombreMascota;
				string tTel = auxProx->telefono;
				string tEsp = auxProx->especie;
				string tMotivo = auxProx->motivoConsulta;
				string tFechaStr = auxProx->fechaString;
				string tHoraStr = auxProx->horaString;
				int tY = auxProx->year;
				int tM = auxProx->month;
				int tD = auxProx->day;
				int tH = auxProx->hour;
				int tMin = auxProx->minutes;
				int tEspInd = auxProx->especieIndex;
				int tPago = auxProx->formaPago;
				float tCosto = auxProx->costo;

				auxProx->nombreDue�o = auxActual->nombreDue�o;
				auxProx->nombreMascota = auxActual->nombreMascota;
				auxProx->telefono = auxActual->telefono;
				auxProx->especie = auxActual->especie;
				auxProx->motivoConsulta = auxActual->motivoConsulta;
				auxProx->fechaString = auxActual->fechaString;
				auxProx->horaString = auxActual->horaString;
				auxProx->year = auxActual->year;
				auxProx->month = auxActual->month;
				auxProx->day = auxActual->day;
				auxProx->hour = auxActual->hour;
				auxProx->minutes = auxActual->minutes;
				auxProx->especieIndex = auxActual->especieIndex;
				auxProx->formaPago = auxActual->formaPago;
				auxProx->costo = auxActual->costo;

				auxActual->nombreDue�o = tNombreD;
				auxActual->nombreMascota = tNombreM;
				auxActual->telefono = tTel;
				auxActual->especie = tEsp;
				auxActual->motivoConsulta = tMotivo;
				auxActual->fechaString = tFechaStr;
				auxActual->horaString = tHoraStr;
				auxActual->year = tY;
				auxActual->month = tM;
				auxActual->day = tD;
				auxActual->hour = tH;
				auxActual->minutes = tMin;
				auxActual->especieIndex = tEspInd;
				auxActual->formaPago = tPago;
				auxActual->costo = tCosto;
			}
			auxProx = auxProx->next;
		}
		auxActual = auxActual->next;
	}
}

void impresion() {
	while (aux != NULL) {
		string display = aux->nombreMascota + "  |  " + aux->fechaString + "  |  " + aux->horaString;
		char buffL[100];
		strcpy(buffL, display.c_str());
		SendMessage(hLbAgenda, LB_ADDSTRING, 0, (LPARAM)buffL);

		aux = aux->next;
	}
	aux = origin;
}
#pragma endregion