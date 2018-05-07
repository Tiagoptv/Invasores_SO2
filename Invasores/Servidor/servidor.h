#pragma once

typedef struct {
	int x, y, vidas, taxaDisparo, velMovimento;
	bool imune, teclasInvertidas;
	Powerup powerups[3];								//Substituir por nMaxPowerups num futuro pr�ximo
	Tiro tiros[10];										//Substituir por nMaxTiros num futuro pr�ximo
}NaveDefensora;

typedef struct {
	int x, y, vida, taxaDisparo, velMovimento;
	Powerup powerups[3];								//Substituir por nMaxPowerups num futuro pr�ximo
	Tiro tiros[10];										//Substituir por nMaxTiros num futuro pr�ximo
}NaveInvasora;

typedef struct {
	int id, duracao;		//id = 1 -> Escudo | id = 2 -> Gelo | id = 3 -> Bateria | id = 4 -> Mais | id = 5 -> Vida | id = 6 -> �lcool  ou algo parecido
}Powerup;

typedef struct {
	int x, y;
}Tiro;


typedef struct {
	int dimX, dimY;
	NaveDefensora navesDefensoras[2];									//Substituir por nMaxNavesDef num futuro pr�ximo
	NaveInvasora navesInvasoras[12];									//Substituir por nMaxNavesInv num futuro pr�ximo
}Jogo;