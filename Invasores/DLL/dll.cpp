#include<stdio.h>
#include<Windows.h>
#include"dll.h"

//Fun��o que vai estar no projeto da gateway, mas vai ficar aqui temporariamente at� que o projeto seja criado
void leJogo(DadosCtrl * cDados, Jogo * jogo) {
	
	WaitForSingleObject(cDados->hEventJogo, INFINITE);
	ResetEvent(cDados->hEventJogo);
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	//jogo = cDados->jogoPartilhado;
	CopyMemory(jogo, cDados->jogoPartilhado, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);
}


//Vai ser usado um evento para avisar que foi posta uma nova mensagem na mem�ria partilhada no ciclo que vai incluir a fun��o de leitura/escrita 
//Leitura -> espera evento e de seguida d� ResetEvent() e l� a mensagem
//Escrita -> escreve mensagem e d� SetEvent()

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