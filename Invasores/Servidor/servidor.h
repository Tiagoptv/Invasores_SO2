#pragma once

typedef struct {
	int id, duracao;		//id = 1 -> Escudo | id = 2 -> Gelo | id = 3 -> Bateria | id = 4 -> Mais | id = 5 -> Vida | id = 6 -> �lcool  ou algo parecido
}Powerup;

typedef struct {
	int x, y;
	char caracter;												// caracter que representa o tiro  por ex: '^' para defensor e '|' para invasor
}Tiro;

typedef struct {
	TCHAR nome[24];
	int x, y, vidas, taxaDisparo, velMovimento;			//maior a taxa de disparo, mais movimentos ter� de fazer antes de disparar
	char caracter;										// caracter que representa a nave
	bool imune, teclasInvertidas;
	Powerup powerups[3];								//Substituir por nMaxPowerups num futuro pr�ximo
	Tiro tiros[10];										//Substituir por nMaxTiros num futuro pr�ximo
}NaveDefensora;

typedef struct {
	int x, y, vida, taxaDisparo, velMovimento;			//maior a taxa de disparo, mais movimentos ter� de fazer antes de disparar
	char direcao;										// 'e'= esquerda 'd'= direira 
	char caracter;										// caracter que representa a nave
	Powerup powerups[3];								//Substituir por nMaxPowerups num futuro pr�ximo
	Tiro bombas[10];									//Substituir por nMaxBombas num futuro pr�ximo
}NaveInvasora;


typedef struct {
	int dimX, dimY, nNavesDef, nNavesInvBasica, nNavesInvEsquiva;
	NaveDefensora navesDefensoras[2];									//Substituir por nMaxNavesDef num futuro pr�ximo
	NaveInvasora navesInvasoras[12];									//Substituir por nMaxNavesInv num futuro pr�ximo
	HANDLE hThreadsNavesInv[4];
}Jogo;