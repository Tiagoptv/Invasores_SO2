#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "..\DLL\dll.h"
#include "servidor.h"

void WINAPI controlaNaveInv(LPVOID params[]);
void gotoxy(int x, int y);


HANDLE hMutexJogo;	//Mutex relativo ao acesso ao jogo por parte das threads das naves invasoras

Jogo setupJogo() {
	int params[2];				//param 1 -> id  | param2 -> tipo       tipo = 1 -> Básica | (int)tipo = 2 -> Esquiva

	Jogo j;
	j.dimX = 500;
	j.dimY = 500;
	j.nNavesInvBasica = 2;
	j.nNavesInvEsquiva = 2;

	for (int i = 0; i < j.nNavesInvBasica; i++)	{																		//lançar threads que vão controlar as naves invasores basicas
		params[0] = i;
		params[1] = 1;
		j.hThreadsNavesInv[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNaveInv, (LPVOID)params, 0, NULL);
		Sleep(500);
	}
	for (int i = 0; i < j.nNavesInvEsquiva; i++) {																		//lançar threads que vão controlar as naves invasores esquiva
		params[0] = j.nNavesInvBasica + i;
		params[1] = 2;
		j.hThreadsNavesInv[j.nNavesInvBasica+i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNaveInv, (LPVOID)params, 0, NULL);
		Sleep(500);
	}
	
	return j;
}

void WINAPI controlaNaveInv(LPVOID params[]) {
	//lógica relativa ao controlo da nave invasora

	//Código apenas para teste do lançamento das threads
	int nMax = 1000;
	int x, y;
	int id = (int)params[0];
	
	x = 0;
	y = id + 6;

	for ( int i = 0; i < nMax; i++)	{
		WaitForSingleObject(hMutexJogo, INFINITE);
		gotoxy(x,y);
		_tprintf(TEXT("Id: %d  n= %d"), id, i);
		ReleaseMutex(hMutexJogo);
		Sleep(25);
	}
}


void gotoxy(int x, int y) {
	static HANDLE hStdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	if (!hStdout)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdout, coord);
}

int main() {
	Jogo j;
	j = setupJogo();
	hMutexJogo = CreateMutex(NULL, FALSE, TEXT("MutexJogo"));

	WaitForMultipleObjects(4, j.hThreadsNavesInv, TRUE, INFINITE);

	for (int i = 0; i < 4; i++)	{
		CloseHandle(j.hThreadsNavesInv[i]);
	}

	return 0;
}