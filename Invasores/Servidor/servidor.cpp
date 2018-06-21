#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <conio.h>
#include "..\DLL\dll.h"
#include "servidor.h"

void WINAPI controlaNaveInv(LPVOID params[]);
void WINAPI gereTBP();
void gotoxy(int x, int y);
Jogo j;
DadosCtrl cDados;
HANDLE hMutexJogo;	//Mutex relativo ao acesso ao jogo por parte das threads das naves invasoras
HANDLE hEventEnviaJogo;


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

	cDados->jogoPartilhado = (Jogo*)MapViewOfFile(cDados->hMapFileJogo, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Jogo));
	if (cDados->jogoPartilhado == NULL) {
		_tprintf(TEXT("Erro ao criar view da memoria!"));
		return 0;
	}

	return TRUE;
}

void escreveJogo(DadosCtrl * cDados, Jogo * jogo) {

	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	cDados->jogoPartilhado = jogo;
	CopyMemory(cDados->jogoPartilhado, jogo, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);

	//Evento para avisar a gateway que o jogo na memória partilhada foi atualizado
	SetEvent(cDados->hEventJogo);
}

Jogo setupJogo() {
	int params[2];				//param 1 -> id  | param2 -> tipo       tipo = 1 -> Básica | (int)tipo = 2 -> Esquiva

	j.dimX = 100;
	j.dimY = 100;
	j.nNavesInvBasica = 5;
	j.nNavesInvEsquiva = 5;
	
	//inicialização tbp
	for (int i = 0; i < 50; i++) {
		j.tbp[i].x = -1;
		j.tbp[i].y = -1;
		j.tbp[i].caracter = ' ';
	}

	//lançar threads que vão controlar as naves invasores basicas
	for (int i = 0; i < j.nNavesInvBasica; i++)	{																		
		params[0] = i;
		params[1] = 1;
		j.hThreadsNavesInv[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNaveInv, (LPVOID)params, 0, NULL);
		Sleep(20);
	}
	
	//lançar threads que vão controlar as naves invasores esquiva
	for (int i = 0; i < j.nNavesInvEsquiva; i++) {																		
		params[0] = j.nNavesInvBasica + i;
		params[1] = 2;
		j.hThreadsNavesInv[j.nNavesInvBasica+i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNaveInv, (LPVOID)params, 0, NULL);
		Sleep(20);
	}

	j.navesDefensoras[0].caracter = 'H';
	j.navesDefensoras[0].imune = FALSE;
	j.navesDefensoras[0].teclasInvertidas = FALSE;
	j.navesDefensoras[0].x = 15;
	j.navesDefensoras[0].y = 10;
	j.navesDefensoras[0].vida = 10;
	j.navesDefensoras[0].velMovimento = 10;		//conferir
	j.navesDefensoras[0].taxaDisparo = 10;
	
	j.comecaJogo = 1;

	return j;
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

//tem o limite de naves e tbp estático, mudar para uma global caso seja necessário
void imprimeCenas() {
	
	int x, y, i;

	//Naves Invasoras
	for (i = 0; i < 50; i++) {
		x = j.navesInvasoras[i].x;
		y = j.navesInvasoras[i].y;

		gotoxy(x,y);

		_tprintf(TEXT("%c "), j.navesInvasoras[i].caracter);
	}

	//Naves Defensoras
	for (i = 0; i < 2; i++) {
		x = j.navesDefensoras[i].x;
		y = j.navesDefensoras[i].y;

		gotoxy(x, y);

		_tprintf(TEXT("%c "), j.navesDefensoras[i].caracter);
	}

	//TBP
	for (i = 0; i < 50; i++) {									///////////////////////////////////////// está a dar zero!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		gotoxy(j.tbp[i].x, j.tbp[i].y);

		_tprintf(TEXT("%c "), j.tbp[i].caracter);
	}
}

void moverInvasor(int id) {

	Sleep(1000);

	if (j.navesInvasoras[id].direcao = 'd') {
		j.navesInvasoras[id].x += 1; //j.navesInvasoras[id].velMovimento;

		//limite lateral
		if (j.navesInvasoras[id].x >= j.dimX) {
			j.navesInvasoras[id].x = j.dimX;
			j.navesInvasoras[id].y++;
			j.navesInvasoras[id].direcao = 'e';
		}
		//limite de baixo
		if (j.navesInvasoras[id].y >= j.dimY) {
			j.navesInvasoras[id].y = 3;
			j.navesInvasoras[id].x = 3 + id;
			j.navesInvasoras[id].direcao = 'e';
		}
	}
	if (j.navesInvasoras[id].direcao = 'e') {
		j.navesInvasoras[id].x -= 1; //j.navesInvasoras[id].velMovimento;

		//limite lateral
		if (j.navesInvasoras[id].x <= 3 || j.navesInvasoras[id].x == 3 + id) {					//adicionar uma função para trocar a direçao
			j.navesInvasoras[id].x = 3 + id;											// de todas as naves, para se mexerem em bloco
			j.navesInvasoras[id].y++;
			j.navesInvasoras[id].direcao = 'd';
		}
		//limite de baixo
		if (j.navesInvasoras[id].y >= j.dimY) {
			j.navesInvasoras[id].y = 3;
			j.navesInvasoras[id].x = 3 + id;
			j.navesInvasoras[id].direcao = 'e';
		}
	}

	j.navesInvasoras[id].nMovs++;


}

void WINAPI controlaNaveInv(LPVOID params[]) {
	//lógica relativa ao controlo da nave invasora
		
	int id = (int)params[0];
	
	//posicao das naves invasoras
	j.navesInvasoras[id].x = 3 + id;
	j.navesInvasoras[id].y = 3;
	j.navesInvasoras[id].nMovs = 0;
	
	if (j.navesInvasoras[id].x > 13 && j.navesInvasoras[id].x <= 23) {
		j.navesInvasoras[id].x -= 10;
		j.navesInvasoras[id].y++;
	}
	if (j.navesInvasoras[id].x > 23 && j.navesInvasoras[id].x <= 33) {
		j.navesInvasoras[id].x -= 20;
		j.navesInvasoras[id].y+=2;
	}
	if (j.navesInvasoras[id].x > 33 && j.navesInvasoras[id].x <= 43) {
		j.navesInvasoras[id].x -= 30;
		j.navesInvasoras[id].y+=3;
	}
	if (j.navesInvasoras[id].x > 43 && j.navesInvasoras[id].x <= 53) {
		j.navesInvasoras[id].x -= 40;
		j.navesInvasoras[id].y+=4;
	}

	//caracteristicas especificas do tipo de nave
	if ((int)params[1] == 1)
	{
		j.navesInvasoras[id].vida = 1;
		j.navesInvasoras[id].taxaDisparo = 10;
		j.navesInvasoras[id].velMovimento = 10;				//ver e modificar como pede no enunciado mais tarde
		j.navesInvasoras[id].direcao = 'd';
		j.navesInvasoras[id].caracter = '@';
		// falta powerups e bombas
	}
	if ((int)params[1] == 2)
	{
		j.navesInvasoras[id].vida = 3;
		j.navesInvasoras[id].taxaDisparo = 16;
		j.navesInvasoras[id].velMovimento = 11;				//ver e modificar como pede no enunciado mais tarde
		j.navesInvasoras[id].direcao = 'd';
		j.navesInvasoras[id].caracter = '#';
		// falta powerups e bombas	
	}

	//mover invasor
	moverInvasor(id);

	//ver se dispara
	if (j.navesInvasoras[id].nMovs == j.navesInvasoras[id].taxaDisparo) {
		j.navesInvasoras[id].nMovs = 0;

		for (int i = 0; i < 50; i++) {
			if(j.tbp[i].x == -1) {							//posição livre
				j.tbp[i].x = j.navesInvasoras[id].x;
				j.tbp[i].y = j.navesInvasoras[id].y + 1;
				j.tbp[i].caracter = '|';
			}
		}
		
		gereTBP();
	}

}

// falta remover em caso de colisao com adversário ou jogador					e fazer while jogoComecou com sleep
void WINAPI gereTBP()
{
	int i, k;

	while (j.comecaJogo) {

		for (i = 0; i < 50; i++) {

			//if (j.tbp[i].x != -1) {

				//bomba dos invasores
				if (j.tbp[i].caracter == '|')
				{
					//if ultrapassou o limite inferior
					if (j.tbp[i].y >= j.dimY)
					{
						j.tbp[i].x = -1;
						j.tbp[i].y = -1;
						j.tbp[i].caracter = ' ';
					}
					//else
					j.tbp[i].y++;
				}

				//tiro dos defensores
				if (j.tbp[i].caracter == '^')
				{
					//ultrapassou os limites superiores
					if (j.tbp[i].y < 4)
					{
						j.tbp[i].x = -1;
						j.tbp[i].y = -1;
						j.tbp[i].caracter = ' ';
					}
					else
						j.tbp[i].y--;
				}

				//powerups
				if (j.tbp[i].caracter == '*')		//bomba dos invasores
				{
					//if ultrapassou o limite inferior
					if (j.tbp[i].y >= j.dimY)
					{
						j.tbp[i].x = -1;
						j.tbp[i].y = -1;
						j.tbp[i].caracter = ' ';
					}
					//else
					j.tbp[i].y++;
				}
			//}
		}
		Sleep(500);
	}
}

// Associar à naveDefensora
void WINAPI readConsoleInput() {

	Mensagem msg;

	int ch;		

	while (j.comecaJogo) {

		//falta set on cliente
		hEventEnviaJogo = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoEnviaJogo"));
		WaitForSingleObject(hEventEnviaJogo, INFINITE);
		CloseHandle(hEventEnviaJogo);
		leMsg(&cDados, &msg);

		ch = _tstoi(msg.mensagem);
	
		//move left
		if (ch == 97) {
			j.navesDefensoras[0].x -= 1;				//mudar para Defensora
		}
		//move right
		if (ch == 100) {
			j.navesDefensoras[0].x += 1;
		}
		//fire
		if (ch == 32) {
				
 			for (int i = 0; i < 50; i++) {
				//posição livre
				if (j.tbp[i].x == -1) {							
					j.tbp[i].x = j.navesDefensoras[0].x;
					j.tbp[i].y = j.navesDefensoras[0].y - 1;
					j.tbp[i].caracter = '^';
					break;
				}
			}
			gereTBP();
		}
	}
}

//nao tem em conta imunidades
//colisoes com powerups nao implementadas
void colisaoTBP() {

	int i, k;

	//Naves Invasoras
	for(i = 0; i < 50; i++)
		for(k = 0; k < 50; k++)
			if (j.navesInvasoras[i].caracter != ' ' && 
				j.navesInvasoras[i].x == j.tbp[k].x &&
				j.navesInvasoras[i].y == j.tbp[k].y) {
				
				//remove tiro do vetor
				j.tbp[k].x = -1;
				j.tbp[k].y = -1;
				j.tbp[k].caracter = ' ';

				//retira uma vida
				j.navesInvasoras[i].vida--;

				//if (sem vidas) -> morre
				if (j.navesInvasoras[i].vida < 1) {
					j.navesInvasoras[k].x = 0;
					j.navesInvasoras[k].y = 0;
					j.navesInvasoras[k].caracter = ' ';
				}
			}

	//Naves defensoras
	for (k = 0; k < 2; k++)
		for (i = 0; i < 50; i++)
			if (j.navesDefensoras[k].caracter != ' ' &&
				j.navesDefensoras[k].x == j.tbp[i].x &&
				j.navesDefensoras[k].y == j.tbp[i].y) {

				//remove tiro do vetor
				j.tbp[i].x = -1;
				j.tbp[i].y = -1;
				j.tbp[i].caracter = ' ';

				//retira uma vida
				j.navesDefensoras[k].vida--;

				//if (sem vidas) -> morre
				if (j.navesDefensoras[k].vida < 1)
					j.navesDefensoras[k].caracter = ' ';
					
			}

}

// nº de invasores fixo
void fimJogo() {

	int i,k;

	if (j.navesDefensoras[0].vida == 0) {
		j.comecaJogo = 0;
		return;
	}

	k = 0;

	for (i = 0; i < 50; i++)
		if (j.navesInvasoras[i].x == 0 && j.navesInvasoras[i].y == 0)
			k++;
		else
			break;

	if (k == 50)
		j.comecaJogo = 0;
}

int main() {
	
	j.comecaJogo = 0;
	HANDLE hThreadTiros, hThreadPlayerMov;
	srand(time(NULL));
	j = setupJogo();

	iniciaMemJogo(&cDados);
	iniciaMemMsg(&cDados);

	// conectar jogadores
	// setup jogo
	// Ciclo de jogo (flag começaJogo)
		// "ativar" thread ler teclas
	// condiçoes de paragem

	//free's e close's
	hThreadPlayerMov = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readConsoleInput, NULL, 0, NULL);
	hThreadTiros = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gereTBP, NULL, 0, NULL);

	int i = 0;
  	while (j.comecaJogo) {

		colisaoTBP();
		imprimeCenas();
		fimJogo();
		//escreveJogo(&cDados, &j);
	/*	gotoxy(100, 1);
		_tprintf(TEXT("%i "), i);
		i++;*/
		//Sleep(200);
	}

	WaitForMultipleObjects(4, j.hThreadsNavesInv, TRUE, INFINITE);

	for (int i = 0; i < 4; i++)	{
		CloseHandle(j.hThreadsNavesInv[i]);
	}

	CloseHandle(hThreadTiros);
	CloseHandle(hThreadPlayerMov);
	CloseHandle(cDados.hMutexJogo);
	CloseHandle(cDados.hEventJogo);
	UnmapViewOfFile(cDados.hMapFileJogo);

	return 0;
}