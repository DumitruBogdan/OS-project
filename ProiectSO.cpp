#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#define MAXNR 1000

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "ProiectSO";
	HWND        hwnd;
	MSG         msg;
	WNDCLASSEX  wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;  //optiuni pentru stilul clasei 
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// incarca pictograma fisierelor de tip aplicatie
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // incarca cursorul sageata  
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// fondul ferstrei de culoare alba     
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName, //numele clasei inregistrat cu RegisterClass
		"Operatii pe multimi",    // text pentru bara de titlu a ferestrei
		WS_OVERLAPPEDWINDOW,   // stilul ferestrei
		CW_USEDEFAULT,      // pozitia orizontala implicitã
		CW_USEDEFAULT,      // pozitia verticala implicita
		CW_USEDEFAULT,       // latimea implicita
		CW_USEDEFAULT,       // inaltimea implicita
		NULL,               // handle-ul ferestrei parinte
		NULL,               // handle-ul meniului ferestrei
		hInstance,          // proprietara ferestrei 
		NULL);


	SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE);
	// schimba dimensiunea, pozitia si ordinea z a ferestrei copil, a ferestrei pop-up
	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	hwndMain = hwnd;



	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	switch (iMsg)
	{
	case WM_CREATE: // operatiile ce se executa la crearea ferestrei
	   // se creaza caseta de dialog
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		if (!dlgActive) {
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1),
				hwnd, DlgProc);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			// insereaza un nou mesaj nou in coada de asteptare
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0); // insereaza un mesaj de incheiere 
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


int A[MAXNR], B[MAXNR];
char Rezultatul1[5 * MAXNR], Rezultatul2[5 * MAXNR], Rezultatul3[5 * MAXNR];
int n, m;

// Functie pentru gasirea unui element in vector
bool GasesteNumarInMultime(int array[MAXNR], int CeValoare, int Dimensiunea) {
	//Se parcurge vectorul array pana la Dimensiunea sau pana gaseste CeValoare
	for (int i = 0; i < Dimensiunea; i++)
		if (CeValoare == array[i])
			return 1;// S-a gasit
			//Daca CeValoare este mai mica decat array[i], asta inseamna ca nu se va mai gasi CeValoare de la acest i pana la sfarsit,
			// deoarece vectorul este sortat crescator
		else if (CeValoare < array[i])
				return 0;
	return 0;
}

// Thread 1 - responsabil pentru A intersectat cu B
DWORD WINAPI Thread1(LPVOID R1) {
	// Se cauta acele elemente din A care sunt si in B
	for (int i = 0; i < n; i++)
		// Daca s-a gasit, se insereaza in solutie
		 if (GasesteNumarInMultime(B, A[i], m) == 1)
			 sprintf(Rezultatul1, "%s %d", Rezultatul1, A[i]);	

	return (DWORD)R1;
}

// Threadul 2 - responsabil pentru A reunit cu B
DWORD WINAPI Thread2(LPVOID R2) {
	// Se insereaza mai intai toata multimea A
	for (int i = 0; i < n; i++)
		sprintf(Rezultatul2, "%s %d", Rezultatul2, A[i]);
	// Apoi se insereaza acele elemente din B care nu sunt in A
	for (int i = 0; i < m; i++)
		if (GasesteNumarInMultime(A, B[i], n) == 0)
			sprintf(Rezultatul2, "%s %d", Rezultatul2, B[i]);

	return (DWORD)R2;
}

// Threadul 3 - responsabil pentru A-B
DWORD WINAPI Thread3(LPVOID R3) {
	// Se parcurge prima multime
	for (int i = 0; i < n; i++)
		// Daca nu s-a gasit numarul atunci este adaugat in solutie
		if (GasesteNumarInMultime(B, A[i], m) == 0)
			sprintf(Rezultatul3, "%s %d", Rezultatul3, A[i]);
	return (DWORD)R3;
}


// Threadul principal
DWORD WINAPI MainThread(LPVOID q) {
	// Handle-urile threadurilor secundare
	HANDLE Threads[4];
	DWORD idThread[4];
	// S-a preluat hDlg pentru a face afisarea in Thread si nu in DlgProc
	HWND hDlg = (HWND)q;
	// Se creaza cele 3 threaduri

	Threads[0]= CreateThread(NULL, 0,
		Thread1,
		NULL, 0, &idThread[0]);
	Threads[1] = CreateThread(NULL, 0,
		Thread2,
		NULL, 0, &idThread[1]);
	Threads[2] = CreateThread(NULL, 0,
		Thread3,
		NULL, 0, &idThread[2]);

	// Se asteapta finalizarea tuturor
	WaitForMultipleObjects(3, Threads, TRUE, INFINITE);

	// Se afiseaza rezultatele
	SetDlgItemText(hDlg, IDC_EDIT_INT, Rezultatul1);
	SetDlgItemText(hDlg, IDC_EDIT_REU, Rezultatul2);
	SetDlgItemText(hDlg, IDC_EDIT_DIF, Rezultatul3);

	return (DWORD)q;
}

/////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	char aux[10 * MAXNR];
	char* cp_ctoi;
	DWORD threadID;
	HANDLE hThread;

	switch (iMsg) {

	case WM_INITDIALOG:
		return TRUE;

	case WM_CLOSE:
		dlgActive = FALSE;
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_GENERARE:
			// Se preiau dimensiunile celor 2 multimi
			GetDlgItemText(hDlg, IDC_VALORI, aux, 100);

			// Se preia prima dimensiune, n, specifica primei multimi
			cp_ctoi = strtok(aux, " ");

			// O convertim din char in int si se memoreaza in n rezultatul
			n = atoi(cp_ctoi);

			// Se preia a doua dimensiune, m, specifica celei de a doua multimi
			cp_ctoi = strtok(NULL, " ");

			// O convertim din char in int si se memoreaza in n rezultatul
			m = atoi(cp_ctoi);

			// Generarea primei multimi
			for (int i = 0; i < n; i++)
				A[i] = rand();

			// Sa creaza a doua multime si se memoreaza in vectorul v_B
			for (int j = 0; j < m; j++) 
				B[j] = rand();


			// Se sorteaza vectorii
			sort(A, A + n);
			sort(B, B + m);

			// Se afiseaza prima multime in ordine crescatoare
			sprintf(aux, "");
			for (int i = 0; i < n; i++)
				sprintf(aux, ("%s %d"), aux, A[i]);

			SetDlgItemText(hDlg, IDC_EDIT_A, aux);

			//Se afiseaza a doua multime in ordine crescatoare
			sprintf(aux, "");
			for (int j = 0; j < m; j++)
				sprintf(aux, "%s %d", aux, B[j]);

			SetDlgItemText(hDlg, IDC_EDIT_B, aux);

			return TRUE;

		case ID_CALC:
			// Se initializeaza variabilele rezultat cu NULL
			sprintf(Rezultatul1, "");
			sprintf(Rezultatul2, "");
			sprintf(Rezultatul3, "");
			// Se creaza MainThread
			hThread = CreateThread(NULL, 0,
				MainThread,
				hDlg, 0, &threadID);
			return TRUE;

		case ID_CANCEL:
			dlgActive = FALSE;
			EndDialog(hDlg, 0);
			return TRUE;
			}
			break;
		}
		return FALSE;
	}
