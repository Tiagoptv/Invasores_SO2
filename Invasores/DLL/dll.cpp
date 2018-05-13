#include<stdio.h>
#include<Windows.h>
#include"dll.h"

//Função que vai estar no projeto da gateway, mas vai ficar aqui temporariamente até que o projeto seja criado
void leJogo(DadosCtrl * cDados, Jogo * jogo) {
	
	WaitForSingleObject(cDados->hEventJogo, INFINITE);
	ResetEvent(cDados->hEventJogo);
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	//jogo = cDados->jogoPartilhado;
	CopyMemory(jogo, cDados->jogoPartilhado, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);
}



bool iniciaMemMsg(DadosCtrl * cDados) {							// O servidor é que mapeia a memória e cria o mutex. O cliente vai abrir a zona de memória e mutex posteriormente
	
	cDados->hMapFileMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MSG), NOME_FM_MSG);
	if (cDados->hMapFileMsg == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hMutexMsg = CreateMutex(NULL, FALSE, NOME_MUTEX_MSG_MEM);
	if (cDados->hMutexMsg == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hEventMsgFromServer = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoMsgFromServer"));
	if (cDados->hEventMsgFromServer == NULL) {
		_tprintf(TEXT("Erro ao criar o evento relativo a mensagens enviadas pelo servidor! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hEventMsgFromGateway = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoMsgFromGateway"));
	if (cDados->hEventMsgFromGateway == NULL) {
		_tprintf(TEXT("Erro ao criar o evento relativo a mensagens enviadas pela gateway! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}



//Vai ser usado um evento para avisar que foi posta uma nova mensagem na memória partilhada no ciclo que vai incluir a função de leitura/escrita 
//Leitura -> espera evento e de seguida dá ResetEvent() e lê a mensagem
//Escrita -> escreve mensagem e dá SetEvent()

void escreveMsg(DadosCtrl * cDados, MSG_PARTILHADA * msg) {
	
	WaitForSingleObject(cDados->hMutexMsg, INFINITE);
	cDados->msgPartilhada = msg;
	ReleaseMutex(cDados->hMutexMsg);
}

void leMsg(DadosCtrl * cDados, MSG_PARTILHADA * msg) {
	
	WaitForSingleObject(cDados->hMutexMsg, INFINITE);
	CopyMemory(msg, cDados->msgPartilhada, sizeof(MSG_PARTILHADA));
	//msg = cDados->msgPartilhada;
	ReleaseMutex(cDados->hMutexMsg);
}