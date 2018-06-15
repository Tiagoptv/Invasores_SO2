#include<stdio.h>
#include<Windows.h>
#include"dll.h"


//Vai ser usado um evento para avisar que foi posta uma nova mensagem na memória partilhada no ciclo que vai incluir a função de leitura/escrita 
//Leitura -> espera evento e de seguida dá ResetEvent() e lê a mensagem
//Escrita -> escreve mensagem e dá SetEvent()

void escreveMsg(DadosCtrl * cDados, Mensagem * msg) {

	int pos;
	
	WaitForSingleObject(cDados->hSemPodeEscrever, INFINITE);

	WaitForSingleObject(cDados->hMutexIndiceMsg, INFINITE);

	pos = cDados->msgPartilhada->in;
	cDados->msgPartilhada->in++;
	
	ReleaseMutex(cDados->hMutexIndiceMsg);

	CopyMemory(&cDados->msgPartilhada->msg[pos], msg, sizeof(Mensagem));

	ReleaseSemaphore(cDados->hSemPodeLer, 1, NULL);
}

void leMsg(DadosCtrl * cDados, Mensagem * msg) {		//em vez de MSG_PARTILHADA, recebe um ponteiro de um array de TCHAR que vai guardar a mensagem pretendida

	int pos;
	
	WaitForSingleObject(cDados->hSemPodeLer, INFINITE);
	WaitForSingleObject(cDados->hMutexIndiceMsg, INFINITE);

	pos = cDados->msgPartilhada->out;
	cDados->msgPartilhada->out++;
	
	ReleaseMutex(cDados->hMutexIndiceMsg);

	CopyMemory(msg, &cDados->msgPartilhada->msg[pos], sizeof(Mensagem));

	ReleaseSemaphore(cDados->hSemPodeEscrever,1,NULL);
}