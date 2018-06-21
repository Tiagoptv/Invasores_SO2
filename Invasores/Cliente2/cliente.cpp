#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "../Servidor/servidor.h"
#include "../DLL/dll.h"
#include "resource.h"

#define PIPE_JOGO TEXT("\\\\.\\pipe\\jogo")
#define PIPE_MENSAGEM TEXT("\\\\.\\pipe\\mensagem")
#define PIPE_CONFIG TEXT("\\\\.\\pipe\\config")
#define NOME_MUTEX_JOGO TEXT("MUTEX_JOGO")


void iniciaJogo();
void enviaInput(Mensagem m);
void ligaPipes();
void WINAPI recebeJogo();

HANDLE hEventEnviaJogo;

/* ===================================================== */
/* Programa base (esqueleto) para aplica��es Windows */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
// Modelo para programas Windows:
// Composto por 2 fun��es:
// WinMain() = Ponto de entrada dos programas windows
// 1) Define, cria e mostra a janela
// 2) Loop de recep��o de mensagens provenientes do Windows
// TrataEventos()= Processamentos da janela (pode ter outro nome)
// 1) � chamada pelo Windows (callback)
// 2) Executa c�digo em fun��o da mensagem recebida
LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
// Nome da classe da janela (para programas de uma s� janela, normalmente este nome �
// igual ao do pr�prio programa) "szprogName" � usado mais abaixo na defini��o das
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Invasores");
Jogo jogo;
HANDLE hMutexJogo;
HANDLE hPipeJogo, hPipeMensagem;
HBITMAP hNaveInv1, hNaveInv2, hNaveDef;

HDC hdc = NULL;
HDC auxDC = NULL, auxDC2 = NULL;
HBRUSH bg = NULL;
HBITMAP auxBM = NULL;
int nX = 0, nY = 0, x = 0, y = 0, x1 = 250, y1 = 250;
HBITMAP hNave = NULL, hNave2 = NULL;
HBITMAP hFundo = NULL;

// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()que desempenha
// o papel da fun��o main() do C em modo consola WINAPI indica o "tipo da fun��o" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as fun��es de
// processamento da janela)
// Par�metros:
// hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa
// hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
// lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0
// destinada a conter par�metros para o programa
// nCmdShow: Par�metro que especifica o modo de exibi��o da janela (usado em
// ShowWindow()
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd; // hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX � uma estrutura cujos membros servem para
					  // definir as caracter�sticas da classe da janela
					  // ============================================================================
					  // 1. Defini��o das caracter�sticas da janela "wcApp"
					  // (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
					  // ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Inst�ncia da janela actualmente exibida
							 // ("hInst" � par�metro de WinMain e vem
							 // inicializada da�)
	wcApp.lpszClassName = szProgName; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endere�o da fun��o de processamento da janela
									  // ("TrataEventos" foi declarada no in�cio e
									  // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for
										  // modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);// "hIcon" = handler do �con normal
												  //"NULL" = Icon definido no Windows
												  // "IDI_AP..." �cone "aplica��o"
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);// "hIconSm" = handler do �con pequeno		//Criar um icon
													//"NULL" = Icon definido no Windows
													// "IDI_INF..." �con de informa��o
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato)
												 // "NULL" = Forma definida no Windows
												 // "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); // Classe do menu que a janela pode ter
							   // (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por
	// "GetStockObject".Neste caso o fundo ser� branco
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName, // Nome da janela (programa) definido acima
		TEXT("Invasores"),// Texto que figura na barra do t�tulo
		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT, // Posi��o x pixels (default=� direita da �ltima)
		CW_USEDEFAULT, // Posi��o y pixels (default=abaixo da �ltima)
		CW_USEDEFAULT, // Largura da janela (em pixels)
		CW_USEDEFAULT, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
							// outra) ou HWND_DESKTOP se a janela for a primeira,
							// criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da inst�ncia do programa actual ("hInst" �
						  // passado num dos par�metros de WinMain()
		0); // N�o h� par�metros adicionais para a janela
			// ============================================================================
			// 4. Mostrar a janela
			// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
								// "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e.
								// normal/modal); � passado como par�metro de WinMain()
	UpdateWindow(hWnd); // Refrescar a janela (Windows envia � janela uma
						// mensagem para pintar, mostrar dados, (refrescar)�
						// ============================================================================
						// 5. Loop de Mensagens
						// ============================================================================
						// O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa fila de
						// espera at� que GetMessage(...) possa ler "a mensagem seguinte"
						// Par�metros de "getMessage":
						// 1)"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no
						// in�cio de WinMain()):
						// HWND hwnd handler da janela a que se destina a mensagem
						// UINT message Identificador da mensagem
						// WPARAM wParam Par�metro, p.e. c�digo da tecla premida
						// LPARAM lParam Par�metro, p.e. se ALT tamb�m estava premida
						// DWORD time Hora a que a mensagem foi enviada pelo Windows
						// POINT pt Localiza��o do mouse (x, y)
						// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem
						// receber as mensagens para todas as janelas pertencentes � thread actual)
						// 3)C�digo limite inferior das mensagens que se pretendem receber
						// 4)C�digo limite superior das mensagens que se pretendem receber
						// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
						// terminando ent�o o loop de recep��o de mensagens, e o programa
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg); // Pr�-processamento da mensagem (p.e. obter c�digo
								  // ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
								 // aguarda at� que a possa reenviar � fun��o de
								 // tratamento da janela, CALLBACK TrataEventos (abaixo)
	}
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam); // Retorna sempre o par�metro wParam da estrutura lpMsg
}
// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Esta fun��o pode ter um nome qualquer: Apenas � neces�rio que na inicializa��o da
// estrutura "wcApp", feita no in�cio de WinMain(), se identifique essa fun��o. Neste
// caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pr�-processadas
// no loop "while" da fun��o WinMain()
//
// Par�metros:
// hWnd O handler da janela, obtido no CreateWindow()
// messg Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
// wParam O par�metro wParam da estrutura messg (a mensagem)
// lParam O par�metro lParam desta mesma estrutura
//
// NOTA:Estes par�metros est�o aqui acess�veis o que simplifica o acesso aos seus valores
//
// A fun��o EndProc � sempre do tipo "switch..." com "cases" que descriminam a mensagem
// recebida e a tratar. Estas mensagens s�o identificadas por constantes (p.e.
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
//============================================================================
LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	static BITMAP bmNave;
	static HDC hdcNave;
	static BITMAP bmNave2;
	static HDC hdcNave2;
	static BITMAP bmFundo;
	static HDC hdcFundo;

	switch (messg) {


		BOOL t, t1;
		DWORD teste;

	case WM_CREATE:

		hEventEnviaJogo = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoEnviaJogo"));

		// OBTEM AS DIMENSOES DO DISPLAY... 
		bg = CreateSolidBrush(RGB(255, 0, 0));
		nX = GetSystemMetrics(SM_CXSCREEN);
		nY = GetSystemMetrics(SM_CYSCREEN);

		// PREPARA 'BITMAP' E ASSOCIA A UM 'DC' EM MEMORIA... 
		hdc = GetDC(hWnd);
		auxDC = CreateCompatibleDC(hdc);
		auxBM = CreateCompatibleBitmap(hdc, nX, nY);
		SelectObject(auxDC, auxBM);
		SelectObject(auxDC, bg);
		PatBlt(auxDC, 0, 0, nX, nY, PATCOPY);
		ReleaseDC(hWnd, hdc);

		//Desenhar Fundo
		/*hFundo = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hdc = GetDC(hWnd);
		GetObject(hFundo, sizeof(bmFundo), &bmFundo);
		hdcFundo = CreateCompatibleDC(hdc);
		//SelectObject(hdcFundo, hFundo);
		SelectObject(auxDC, hFundo);
		PatBlt(auxDC, 0, 0, nX, nY, PATCOPY);*/

		hNave = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hNave2 = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hdc = GetDC(hWnd);
		GetObject(hNave, sizeof(bmNave), &bmNave);
		GetObject(hNave2, sizeof(bmNave2), &bmNave2);
		hdcNave = CreateCompatibleDC(hdc);
		hdcNave2 = CreateCompatibleDC(hdc);
		SelectObject(hdcNave, hNave);
		SelectObject(hdcNave2, hNave2);
		ReleaseDC(hWnd, hdc);
		break;

	case WM_KEYDOWN:
		// CODIGO ASSOCIADO A TECLA SETA PARA A DIREITA...
		if (wParam == VK_RIGHT) {
			x1++;
			InvalidateRect(hWnd, NULL, FALSE);							//For�a WM_PAINT
		}
		if (wParam == VK_LEFT) {
			x1--;
			InvalidateRect(hWnd, NULL, FALSE);							//For�a WM_PAINT
		}
		break;

	case WM_PAINT:
		// DESENHA NO 'DC' EM MEMORIA... 		
		PatBlt(auxDC, 0, 0, nX, nY, PATCOPY);
		for (int i = 0; i < 20; i++)
		{
			TransparentBlt(auxDC, x + (i*bmNave.bmWidth), y, bmNave.bmWidth, bmNave.bmHeight, hdcNave, 0, 0, bmNave.bmWidth, bmNave.bmHeight, RGB(0x00, 0x00, 0xFF));
		}

		TransparentBlt(auxDC, x1, y1, bmNave2.bmWidth, bmNave.bmHeight, hdcNave2, 0, 0, bmNave2.bmWidth, bmNave.bmHeight, RGB(0x00, 0x00, 0xFF));

		// COPIA INFORMACAO DO 'DC' EM MEMORIA PARA O DISPLAY... 
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, nX, nY, auxDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{

		case ID_JOGO_JOGAR:
			iniciaJogo();
			SetEvent(hEventEnviaJogo);
			break;	

		case ID_JOGO_SAIR:
			PostQuitMessage(0);
			// LIBERTA RECURSO RELATIVOS AO 'BITMAP' E AO 'DC' EM MEMORIA... 
			DeleteObject(bg);
			DeleteObject(auxBM);
			DeleteDC(auxDC);
			break;

		default:
			break;
		}

		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		// LIBERTA RECURSO RELATIVOS AO 'BITMAP' E AO 'DC' EM MEMORIA... 
		DeleteObject(bg);
		DeleteObject(auxBM);
		DeleteDC(auxDC);

		// LIBERTA RECURSOS RELATIVOS AO 'BITMAP' E AO 'DC' DA NAVE... 
		DeleteObject(hNave);
		DeleteDC(hdcNave);

		break;

	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows			
		return(DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return(0);
}

void iniciaJogo() {
	HANDLE hThreadRecebeJogo;

	ligaPipes();

	hThreadRecebeJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recebeJogo, NULL, 0, NULL);

	WaitForSingleObject(hThreadRecebeJogo, INFINITE);
}

void ligaPipes() {

	//Liga Pipe Jogo
	//Espera pelo pipe
	if (!WaitNamedPipe(PIPE_JOGO, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_JOGO);	//Remover depois de testar
		exit(-1);
	}
	//Liga ao pipe
	hPipeJogo = CreateFile(PIPE_JOGO, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipeJogo == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_JOGO);	//Remover depois de testar
		exit(-1);
	}

	//Liga Pipe Mensagem
	//Espera pelo pipe
	if (!WaitNamedPipe(PIPE_MENSAGEM, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_MENSAGEM);	//Remover depois de testar
		exit(-1);
	}
	//Liga ao pipe
	hPipeMensagem = CreateFile(PIPE_MENSAGEM, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipeMensagem == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_MENSAGEM);	//Remover depois de testar
		exit(-1);
	}
}


void loadBitMaps() {
	hNaveInv1 = (HBITMAP)LoadImageW(NULL, L"bitmap3.bmp",IMAGE_BITMAP,0,0, LR_LOADFROMFILE);
}

void WINAPI recebeJogo() {
	DWORD n;
	BOOL ret;

	hMutexJogo = CreateMutex(NULL, FALSE, NOME_MUTEX_JOGO);
	if (hMutexJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		exit(-1);
	}

	//Espera por evento para come�ar a ler o mapa ou s� � lan�ada a thread depois de se dar a ordem para iniciar o jogo

	while (1) {
		WaitForSingleObject(hMutexJogo, INFINITE);
		ret = ReadFile(hPipeJogo, &jogo, sizeof(Jogo), &n, NULL);
		if (!ret || !n) {
			_tprintf(TEXT("[LEITOR] %d %d... (ReadFile)\n"), ret, n);	//Remover depois de testar
			break;
		}
	}
}

void enviaInput(Mensagem m) {
	DWORD n;

	while (1) {
		for (int i = 0; i < 6; i++)
		{
			if (!WriteFile(hPipeMensagem, &m, sizeof(Mensagem), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe n.%d! (WriteFile)\n"), i);	//Remover depois de testar
				exit(-1);
			}
		}
	}
}