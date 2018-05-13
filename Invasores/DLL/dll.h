#pragma once
#include <windows.h>
#include <tchar.h>
#include "../Servidor/servidor.h"

typedef struct {
	TCHAR nomeEmissor[24];
	TCHAR mensagem[12];		//Penso que as mensagens vão ser só para avisar o que o cliente faz (andou para a direita/esquerda, etc...)
}MSG_PARTILHADA;

typedef struct {
	HANDLE hMapFileJogo, hMapFileMsg;
	Jogo * jogoPartilhado;
	MSG_PARTILHADA * msgPartilhada;
	HANDLE hMutexJogo, hMutexMsg, hEventJogo, hEventMsgFromServer, hEventMsgFromGateway;
}DadosCtrl;

#define NOME_MUTEX_JOGO_MEM TEXT("mutexMemJogo")
#define NOME_MUTEX_MSG_MEM TEXT("mutexMemMsg")
#define NOME_FM_MSG TEXT("fmMensagem")
#define NOME_FM_JOGO TEXT("fmJogo")

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C"
{
	//Funções a serem exportadas/importadas
	//Zona de mensagens
	DLL_IMP_API bool iniciaMemMsg(DadosCtrl * cDados);
	DLL_IMP_API void escreveMsg(DadosCtrl * cDados, MSG_PARTILHADA * msg);
	DLL_IMP_API void leMsg(DadosCtrl * cDados, MSG_PARTILHADA * msg);

	DLL_IMP_API void leJogo(DadosCtrl * cDados, Jogo * jogo);
}