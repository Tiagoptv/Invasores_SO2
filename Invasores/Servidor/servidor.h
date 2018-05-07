#pragma once

typedef struct {
	int x, y, vidas, taxaDisparo, velMovimento;
	bool imune, teclasInvertidas;
	Powerup powerups[3];								//Substituir por nMaxPowerups num futuro próximo
	Tiro tiros[10];										//Substituir por nMaxTiros num futuro próximo
}NaveDefensora;

typedef struct {
	int x, y, vida, taxaDisparo, velMovimento;
	Powerup powerups[3];								//Substituir por nMaxPowerups num futuro próximo
	Tiro tiros[10];										//Substituir por nMaxTiros num futuro próximo
}NaveInvasora;

typedef struct {
	int id, duracao;		//id = 1 -> Escudo | id = 2 -> Gelo | id = 3 -> Bateria | id = 4 -> Mais | id = 5 -> Vida | id = 6 -> Álcool  ou algo parecido
}Powerup;

typedef struct {
	int x, y;
}Tiro;


typedef struct {
	int dimX, dimY;
	NaveDefensora navesDefensoras[2];									//Substituir por nMaxNavesDef num futuro próximo
	NaveInvasora navesInvasoras[12];									//Substituir por nMaxNavesInv num futuro próximo
}Jogo;