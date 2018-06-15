#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include "..\DLL\dll.h"
#include "servidor.h"

void WINAPI controlaNaveInv(LPVOID params[]);
void WINAPI gereTBP();
void gotoxy(int x, int y);
Jogo j;
DadosCtrl cDados;
HANDLE hMutexJogo;	//Mutex relativo ao acesso ao jogo por parte das threads das naves invasoras

<<<<<<< HEAD
HANDLE hMutexJogo;	//Mutex relativo ao acesso ao jogo por parte das threads das naves invasoras
=======


//Memória Partilhada
bool iniciaMemMsg(DadosCtrl * cDados) {							// O servidor é que mapeia a memória e cria o mutex. O cliente vai abrir a zona de memória e mutex posteriormente

	cDados->hMapFileMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MSG_PARTILHADA), NOME_FM_MSG);
	if (cDados->hMapFileMsg == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hMutexIndiceMsg = CreateMutex(NULL, FALSE, TEXT("mutexMsgOut"));
	if (cDados->hMutexIndiceMsg == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hSemPodeEscrever = CreateSemaphore(NULL, N_SLOTS_MSG, N_SLOTS_MSG, NOME_SEM_PODE_ESCREVER);
	if (cDados->hSemPodeEscrever == NULL) {
		_tprintf(TEXT("Erro ao criar o semaforo relativo a escrever no buffer! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hSemPodeLer = CreateSemaphore(NULL, N_SLOTS_MSG, N_SLOTS_MSG, NOME_SEM_PODE_LER);
	if (cDados->hSemPodeLer == NULL) {
		_tprintf(TEXT("Erro ao criar o evento relativo a mensagens enviadas pela gateway! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}
>>>>>>> 4ca8e5d4341fad4240d5cfda83cffa273a8f7af8


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

<<<<<<< HEAD
=======
void WINAPI resolveMensagens() {
	Mensagem msg;

	while (1) {
		leMsg(&cDados, &msg);

		//lógica de jogo
	}
}



>>>>>>> 4ca8e5d4341fad4240d5cfda83cffa273a8f7af8
Jogo setupJogo() {
	int params[2];				//param 1 -> id  | param2 -> tipo       tipo = 1 -> Básica | (int)tipo = 2 -> Esquiva

	//Jogo j;
	j.dimX = 100;
	j.dimY = 100;
	j.nNavesInvBasica = 2;
	j.nNavesInvEsquiva = 2;
	j.tbp = (Tiro *)malloc(1);				//1 byte apenas para fazer uma divisao inteira depois e dar 0

	//verificação do malloc
	if (j.tbp == NULL) {																								
		//fprintf(stderr, "malloc failed\n");
		_tprintf(TEXT("malloc failed"));
		return(j);
	}

	//lançar threads que vão controlar as naves invasores basicas
	for (int i = 0; i < j.nNavesInvBasica; i++)	{																		
		params[0] = i;
		params[1] = 1;
		j.hThreadsNavesInv[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNaveInv, (LPVOID)params, 0, NULL);
		Sleep(500);
	}
	
	//lançar threads que vão controlar as naves invasores esquiva
	for (int i = 0; i < j.nNavesInvEsquiva; i++) {																		
		params[0] = j.nNavesInvBasica + i;
		params[1] = 2;
		j.hThreadsNavesInv[j.nNavesInvBasica+i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNaveInv, (LPVOID)params, 0, NULL);
		Sleep(500);
	}
	
	return j;
}

void WINAPI controlaNaveInv(LPVOID params[]) {
	//lógica relativa ao controlo da nave invasora

	int nMovs = 0;					//Contador de movimentos p/ taxa de disparo
	
	NaveInvasora naveInv;

	//posicao das naves invasoras
	naveInv.x = 3 + (int)params[0];
	naveInv.y = 3;
	
	if (naveInv.x > 13 && naveInv.x <= 23) {
		naveInv.x -= 10;
		naveInv.y++;
	}
	if (naveInv.x > 23 && naveInv.x <= 33) {
		naveInv.x -= 20;
		naveInv.y+=2;
	}
	if (naveInv.x > 33 && naveInv.x <= 43) {
		naveInv.x -= 30;
		naveInv.y+=3;
	}
	if (naveInv.x > 43 && naveInv.x <= 53) {
		naveInv.x -= 40;
		naveInv.y+=4;
	}

	//caracteristicas especificas do tipo de nave
	if ((int)params[1] == 1)
	{
		naveInv.vida = 1;
		naveInv.taxaDisparo = 10;
		naveInv.velMovimento = 10;				//ver e modificar como pede no enunciado mais tarde
		naveInv.direcao = 'd';
		naveInv.caracter = '@';
		// falta powerups e bombas
	}
	if ((int)params[1] == 2)
	{
		naveInv.vida = 3;
		naveInv.taxaDisparo = 16;
		naveInv.velMovimento = 11;				//ver e modificar como pede no enunciado mais tarde
		naveInv.direcao = 'd';
		naveInv.caracter = '#';
		// falta powerups e bombas	
	}

	//mover invasor
	if (naveInv.direcao = 'd') {
		naveInv.x += naveInv.velMovimento;
		
		//limite lateral
		if (naveInv.x >= j.dimX) {							
			naveInv.x = j.dimX;
			naveInv.y++;
			naveInv.direcao = 'e';
		}
		//limite de baixo
		if (naveInv.y >= j.dimY) {
			naveInv.y = 3;
			naveInv.x = 3 + (int)params[0];
			naveInv.direcao = 'e';
		}
	}
	if (naveInv.direcao = 'e') {
		naveInv.x -= naveInv.velMovimento;
		
		//limite lateral
		if (naveInv.x <= 3 || naveInv.x == 3 + (int)params[0]) {					//adicionar uma função para trocar a direçao
			naveInv.x = 3 + (int)params[0];											// de todas as naves, para se mexerem em bloco
			naveInv.y++;
			naveInv.direcao = 'd';
		}
		//limite de baixo
		if (naveInv.y >= j.dimY) {
			naveInv.y = 3;
			naveInv.x = 3 + (int)params[0];
			naveInv.direcao = 'e';
		}
	}
	++nMovs;

	//ver se dispara
	if (nMovs == naveInv.taxaDisparo) {
		nMovs = 0;
		
		int pos = sizeof(j.tbp) / sizeof(Tiro);				//numero de elementos; confirmar se temos o resultado esperado
		
		j.tbp = (Tiro*)realloc(j.tbp, (pos+1)*sizeof(Tiro));

		j.tbp[pos].x = naveInv.x;
		j.tbp[pos].x = naveInv.x;
		j.tbp[pos].caracter = '|';		
		
		gereTBP();
	}

	//Código apenas para teste do lançamento das threads e teste da memória partilhada
	/*int nMax = 1000;
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
	UnmapViewOfFile(cDados.hMapFileJogo);*/

}
////////////////////////////////////////////////// rever e acabar a parte da remoçao do array /////////////////////////////////// 
void gereTBP()
{
	int i, k;
	int pos = sizeof(j.tbp) / sizeof(Tiro);					//numero de elementos

	for (i = 0; i > pos; i++) {
	
		//bomba dos invasores
		if(j.tbp[i].caracter == '|')		
		{
			if (j.tbp[i].y >= j.dimY)								//se ultrapassou o limite inferior
			{
				for (k = i; k > pos; k++) {							// percorre o array de i para a frente
					j.tbp[k] = j.tbp[k + 1];						// remover o tiro
				}
				j.tbp = (Tiro*)realloc(j.tbp, (pos - 1) * sizeof(Tiro));
			}		//passar tudo pra casa anterior e realocar o array
			
			j.tbp[i].y++;
		}

		//tiro dos defensores
		if (j.tbp[i].caracter == '^')		
		{
			//codigo para quando sai dos limites superiores
			j.tbp[i].y--;
		}

		//powerups
		if (j.tbp[i].caracter == '*')		//bomba dos invasores
		{
			//codigo para quando sai dos limites inferiores
			j.tbp[i].y++;
		}
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

	//leJogo(&cDados, &j);

	return;
}

int main() {

	HANDLE hThreadTiros;
	srand(time(NULL));
	j = setupJogo();
	hMutexJogo = CreateMutex(NULL, FALSE, TEXT("MutexJogo"));


	iniciaMemJogo(&cDados);

	cDados.jogoPartilhado = (Jogo*)MapViewOfFile(cDados.hMapFileJogo, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Jogo));
	if (cDados.jogoPartilhado == NULL) {
		_tprintf(TEXT("Erro ao criar view da memoria!"));
		return 0;
	}

	hThreadTiros = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gereTBP, NULL, 0, NULL);
	

	// conectar jogadores
	// setup jogo
	// Ciclo de jogo
	// condiçoes de paragem

	//free's e close's


	escreveJogo(&cDados, &j);

	j.hThreadsNavesInv[4] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)testeMem, NULL, 0, NULL);


	WaitForMultipleObjects(4, j.hThreadsNavesInv, TRUE, INFINITE);

	for (int i = 0; i < 4; i++)	{
		CloseHandle(j.hThreadsNavesInv[i]);
	}

	free(j.tbp);
	CloseHandle(hThreadTiros);
	CloseHandle(cDados.hMutexJogo);
	CloseHandle(cDados.hEventJogo);
	UnmapViewOfFile(cDados.hMapFileJogo);

	return 0;
}