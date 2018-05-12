#include<stdio.h>
#include<Windows.h>
#include"dll.h"


bool iniciaMemJogo(dadosCtrl * cDados) {						// O servidor é que mapeia a memória e cria o mutex. O cliente vai abrir a zona de memória e mutex posteriormente
	
	cDados->hMapFileJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Jogo), TEXT("fmJogo"));
	if (cDados->hMapFileJogo == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"),GetLastError());
		return FALSE;
	}

	cDados->hMutexJogo = CreateMutex(NULL, FALSE, NOME_MUTEX_JOGO_MEM);
	if (cDados->hMutexJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

void escreveJogo(dadosCtrl * cDados, Jogo * jogo) {
	
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	cDados->jogoPartilhado = jogo;
	ReleaseMutex(cDados->hMutexJogo);

	//Evento para avisar a gateway que o jogo na memória partilhada foi atualizado
	
}

void leJogo(dadosCtrl * cDados, Jogo * jogo) {
	
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	jogo = cDados->jogoPartilhado;
	ReleaseMutex(cDados->hMutexJogo);
}






bool iniciaMemMsg(dadosCtrl * cDados) {							// O servidor é que mapeia a memória e cria o mutex. O cliente vai abrir a zona de memória e mutex posteriormente
	
	cDados->hMapFileMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MSG), TEXT("fmMensagem"));
	if (cDados->hMapFileMsg == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hMutexMsg = CreateMutex(NULL, FALSE, NOME_MUTEX_MSG_MEM);
	if (cDados->hMutexMsg == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

void escreveMsg(dadosCtrl * cDados, MSG_PARTILHADA * msg) {
	
	WaitForSingleObject(cDados->hMutexMsg, INFINITE);
	cDados->msgPartilhada = msg;
	ReleaseMutex(cDados->hMutexMsg);

	//Evento para avisar que foi posta uma nova mensagem na memória partilhada
}

void leMsg(dadosCtrl * cDados, MSG_PARTILHADA * msg) {
	
	WaitForSingleObject(cDados->hMutexMsg, INFINITE);
	msg = cDados->msgPartilhada;
	ReleaseMutex(cDados->hMutexMsg);
}