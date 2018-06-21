#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include "..\DLL\dll.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

HANDLE hPipes[6], hEventEnviaJogo;
DadosCtrl cDados;

void leJogo(DadosCtrl * cDados, Jogo * jogo) {

	WaitForSingleObject(cDados->hEventJogo, INFINITE);
	ResetEvent(cDados->hEventJogo);
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	CopyMemory(jogo, cDados->jogoPartilhado, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);
}

void WINAPI trataCliente(LPVOID * hP) {

	HANDLE hPipe = (HANDLE)hP;

	Mensagem msg;

	TCHAR buf[24];
	BOOL ret;
	DWORD n;

	//lê nome do cliente
	ret = ReadFile(hPipe, msg.nomeEmissor, sizeof(TCHAR) * 24, &n, NULL);
	msg.nomeEmissor[n / sizeof(TCHAR)] = '\0';
	if (!ret || !n) {
		_tprintf(TEXT("[ERRO] %d %d... (ReadFile)\n"), ret, n);
		exit(-1);
	}


	//lê input do cliente
	while (1) {
		ret = ReadFile(hPipe, msg.mensagem, sizeof(TCHAR) * 24, &n, NULL);
		buf[n / sizeof(TCHAR)] = '\0';
		if (!ret || !n) {
			_tprintf(TEXT("[ERRO] %d %d... (ReadFile)\n"), ret, n);
			break;
		}

		escreveMsg(&cDados, &msg);

	}
}


void WINAPI recebeCliente() {
	HANDLE hPipeAux;

	int index;

	while (1) {
		do
		{
			for (int i = 0; i < 6; i++) {
				if (hPipes[i] == INVALID_HANDLE_VALUE) {
					index = i;
				}
			}
		} while (index == -1);
	}

	hPipeAux = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 6, sizeof(Jogo), sizeof(Jogo), 1000, NULL);
	if (hPipeAux == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
		exit(-1);
	}

	if (!ConnectNamedPipe(hPipeAux, NULL)) {
		_tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe\n"));
		exit(-1);
	}

	hPipes[index] = hPipeAux;

<<<<<<< HEAD
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recebeInputCliente, &hPipesMensagem[index], 0, NULL);

=======
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)trataCliente, &hPipes[index], 0, NULL);
>>>>>>> 580a78351142ef541ddc5fd6be15d0c1649f65e9
}


void WINAPI enviaJogo() {
	DWORD n;
	Jogo j;


	while (1) {
		for (int i = 0; i < 6; i++)
		{
			leJogo(&cDados, &j);
<<<<<<< HEAD
			if (hPipesJogo[i] != INVALID_HANDLE_VALUE) {
				if (!WriteFile(hPipesJogo[i], &j,sizeof(Jogo), &n, NULL)) {
					_tprintf(TEXT("[ERRO] Escrever no pipe n.%d (%d)! (WriteFile)\n"), i, GetLastError());
=======
			if (hPipes[i] == INVALID_HANDLE_VALUE) {
				if (!WriteFile(hPipes[i], &j,sizeof(Jogo), &n, NULL)) {
					_tprintf(TEXT("[ERRO] Escrever no pipe n.%d! (WriteFile)\n"), i);
>>>>>>> 580a78351142ef541ddc5fd6be15d0c1649f65e9
					exit(-1);
				}
			}
		}
	}
<<<<<<< HEAD
}

void WINAPI recebeInputCliente(LPVOID * hP) {
	DWORD n;
	BOOL ret;
	HANDLE hPipe = (HANDLE)hP;
	Mensagem msg;
	TCHAR buf[24];

	//Espera por evento para começar a ler as mensagens

	//lê nome do cliente	| Poderá ser retirado daqui
	/*ret = ReadFile(hPipe, msg.nomeEmissor, sizeof(TCHAR) * 24, &n, NULL);
	msg.nomeEmissor[n / sizeof(TCHAR)] = '\0';
	if (!ret || !n) {
		_tprintf(TEXT("[ERRO] %d %d... (ReadFile)\n"), ret, n);
		exit(-1);
	}*/

	//Espera pelo evento para começar a ler
=======
>>>>>>> 580a78351142ef541ddc5fd6be15d0c1649f65e9

}


int _tmain(int argc, LPSTR argv[]) {

	HANDLE hTRecebeCliente, hTEnviaJogo, hTrataCliente[6];

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	cDados.hEventJogo = OpenEvent(SYNCHRONIZE, TRUE, TEXT("EventoJogo"));
	if (cDados.hEventJogo == NULL) {
		_tprintf(TEXT("Erro ao abrir evento! (%d)"), GetLastError());
		return 0;
	}
	cDados.hMapFileJogo = OpenFileMapping(FILE_MAP_READ, FALSE, NOME_FM_JOGO);
	if (cDados.hMapFileJogo == NULL) {
		_tprintf(TEXT("Erro ao abrir memória partilhada mapeada! (%d)"), GetLastError());
		return 0;
	}

	cDados.hMutexJogo = OpenMutex(SYNCHRONIZE, FALSE, NOME_MUTEX_JOGO_MEM);
	if (cDados.hMutexJogo == NULL) {
		_tprintf(TEXT("Erro ao abrir mutex! (%d)"), GetLastError());
		return 0;
	}

	cDados.jogoPartilhado = (Jogo*) MapViewOfFile(cDados.hMapFileJogo, FILE_MAP_READ,0,0,sizeof(Jogo));


	for (int i = 0; i < 6; i++)
	{
		hPipes[i] = INVALID_HANDLE_VALUE;
	}

	hTRecebeCliente = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recebeCliente, NULL, 0, NULL);
	WaitForSingleObject(hTRecebeCliente, INFINITE);

<<<<<<< HEAD

	hTEnviaJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)enviaJogo, NULL, 0, NULL);	
	WaitForSingleObject(hTEnviaJogo, INFINITE);
=======
	hEventEnviaJogo = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoEnviaJogo"));
	if (hEventEnviaJogo == NULL) {
		_tprintf(TEXT("Erro ao abrir evento! (%d)"), GetLastError());
		return 0;
	}
	
	hTEnviaJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)enviaJogo, NULL, 0, NULL);


	
>>>>>>> 580a78351142ef541ddc5fd6be15d0c1649f65e9
}