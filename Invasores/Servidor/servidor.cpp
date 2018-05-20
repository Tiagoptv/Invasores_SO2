#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "..\DLL\dll.h"
#include "servidor.h"

void WINAPI controlaNaveInv(LPVOID params[]);
void gotoxy(int x, int y);


HANDLE hMutexJogo;	//Mutex relativo ao acesso ao jogo por parte das threads das naves invasoras

bool iniciaMemJogo(DadosCtrl * cDados) {						// O servidor é que mapeia a memória e cria o mutex. O cliente vai abrir a zona de memória e mutex posteriormente

	cDados->hMapFileJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Jogo), NOME_FM_JOGO);
	if (cDados->hMapFileJogo == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hMutexJogo = CreateMutex(NULL, FALSE, NOME_MUTEX_JOGO_MEM);
	if (cDados->hMutexJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hEventJogo = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoJogo"));
	if (cDados->hEventJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o evento relativo ao jogo! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

void escreveJogo(DadosCtrl * cDados, Jogo * jogo) {

	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	//cDados->jogoPartilhado = jogo;
	CopyMemory(cDados->jogoPartilhado, jogo, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);

	//Evento para avisar a gateway que o jogo na memória partilhada foi atualizado
	SetEvent(cDados->hEventJogo);
}


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

	//Código apenas para teste do lançamento das threads e teste da memória partilhada
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

	DadosCtrl cDados;
	//Jogo j;

	cDados.hMapFileJogo = OpenFileMapping(FILE_MAP_READ, FALSE, NOME_FM_JOGO);
	cDados.hMutexJogo = OpenMutex(SYNCHRONIZE, FALSE, NOME_MUTEX_JOGO_MEM);

	WaitForSingleObject(cDados.hMutexJogo,INFINITE);
	ReleaseMutex(cDados.hMutexJogo);


	CloseHandle(cDados.hMutexJogo);
	UnmapViewOfFile(cDados.hMapFileJogo);

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

// função apenas para teste
// apagar depois de testar
void WINAPI testeMem() {
	DadosCtrl cDados;
	Jogo j;

	cDados.hMapFileJogo = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NOME_FM_JOGO);
	if (cDados.hMapFileJogo == NULL) {
		_tprintf(TEXT("Erro ao abrir memoria mapeada!"));
		return;
	}

	cDados.jogoPartilhado = (Jogo*)MapViewOfFile(cDados.hMapFileJogo, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Jogo));
	if (cDados.jogoPartilhado == NULL) {
		_tprintf(TEXT("Erro ao abrir vista do jogo partilhado!"));
		return;
	}

	leJogo(&cDados, &j);

	return;
}

int main() {

	Jogo j;
	j = setupJogo();
	hMutexJogo = CreateMutex(NULL, FALSE, TEXT("MutexJogo"));


	DadosCtrl cDados;

	iniciaMemJogo(&cDados);

	cDados.jogoPartilhado = (Jogo*)MapViewOfFile(cDados.hMapFileJogo, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Jogo));
	if (cDados.jogoPartilhado == NULL) {
		_tprintf(TEXT("Erro ao criar view da memoria!"));
		return 0;
	}


	escreveJogo(&cDados, &j);

	j.hThreadsNavesInv[4] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)testeMem, NULL, 0, NULL);


	WaitForMultipleObjects(4, j.hThreadsNavesInv, TRUE, INFINITE);

	for (int i = 0; i < 4; i++)	{
		CloseHandle(j.hThreadsNavesInv[i]);
	}

	CloseHandle(cDados.hMutexJogo);
	CloseHandle(cDados.hEventJogo);
	UnmapViewOfFile(cDados.hMapFileJogo);

	return 0;
}