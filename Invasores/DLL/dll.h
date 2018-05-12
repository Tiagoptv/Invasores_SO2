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
	HANDLE hMutexJogo, hMutexMsg;
}dadosCtrl;

#define NOME_MUTEX_JOGO_MEM TEXT("mutexMemJogo")
#define NOME_MUTEX_MSG_MEM TEXT("mutexMemJogo")

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C"
{
	//Funções a serem exportadas/importadas

	//Zona de dados do jogo
	DLL_IMP_API bool iniciaMemJogo(dadosCtrl * cDados);
	DLL_IMP_API void escreveJogo(dadosCtrl * cDados, Jogo * jogo);
	DLL_IMP_API void leJogo(dadosCtrl * cDados, Jogo * jogo);

	//Zona de mensagens
	DLL_IMP_API bool iniciaMemMsg(dadosCtrl * cDados);
	DLL_IMP_API void escreveMsg(dadosCtrl * cDados, MSG_PARTILHADA * msg);
	DLL_IMP_API void leMsg(dadosCtrl * cDados, MSG_PARTILHADA * msg);
}