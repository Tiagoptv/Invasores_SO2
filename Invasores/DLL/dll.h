#pragma once
#include <windows.h>
#include <tchar.h>
#include "../Servidor/servidor.h"

typedef struct {
	TCHAR nomeEmissor[24];
	TCHAR mensagem[24];		//Penso que as mensagens vão ser só para avisar o que o cliente faz (andou para a direita/esquerda, etc...)
}Mensagem;

typedef struct {
	Mensagem msg[12];
	int in, out;
}MSG_PARTILHADA;

typedef struct {
	HANDLE hMapFileJogo, hMapFileMsg;
	Jogo * jogoPartilhado;
	MSG_PARTILHADA * msgPartilhada;
	HANDLE hMutexJogo, hMutexIndiceMsg, hEventJogo, hSemPodeEscrever, hSemPodeLer;
}DadosCtrl;

#define NOME_MUTEX_JOGO_MEM TEXT("mutexMemJogo")
#define NOME_FM_MSG TEXT("fmMensagem")
#define NOME_FM_JOGO TEXT("fmJogo")
#define N_SLOTS_MSG 12
#define NOME_SEM_PODE_LER TEXT("semaforoMsgPodeLer")
#define NOME_SEM_PODE_ESCREVER TEXT("semaforoMsgPodeEscrever")

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C"
{
	//Funções a serem exportadas/importadas
	//Zona de mensagens
	DLL_IMP_API void escreveMsg(DadosCtrl * cDados, Mensagem * msg);
	DLL_IMP_API void leMsg(DadosCtrl * cDados, Mensagem * msg);
}