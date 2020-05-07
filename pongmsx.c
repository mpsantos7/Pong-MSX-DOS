/*
PONG MSX-DOS
Author: Marcelo da P. Dos Santos
Some code from Eric Boez.

Stable version
ToDo: Two Players, difficult level,
interrupts.
*/

#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_sprites.h"
#include "fusion-c/header/vdp_graph2.h"
#include <stdlib.h>
#include <string.h>

#define XRIG 240 
#define XLEF 8
#define YTOP 16  
#define YLOW 192

// structs modificado 
struct mysprite{ 
	unsigned char x,y;
	unsigned char nn;
	unsigned char pat;
	unsigned char linecolors[8];
} TheBall, PadLeft, PadRight;
	

//Global Vars
static signed char DirX;					// Direction of the ball
static signed char DirY;					// 
char LeftScore=0,RightScore=0;
//const signed char moves[9]={0,-1,0,0,0,1,0,0,0};//y

const unsigned char PatternBall[]={
						0b00111100,
						0b01111110,
						0b11011111,
						0b11110111,
						0b11101111,
						0b11111011,
						0b01111110,
						0b00111100
};

const unsigned char PatternPad[]={
						0b11111100,
						0b11111100,
						0b11111100,
						0b11111100,
						0b11111100,
						0b11111100,
						0b11111100,
						0b11111100
};


void DesenhaCampo(void){
	char x,y,Dx,Dy;
	Dx=(XRIG-XLEF)/8;
	Dy=(YLOW-YTOP)/8;
	SetColors(15,0,3);
	for(x=XLEF+2;x<XRIG-8;x=x+2*Dx)
		for(y=YTOP;y<YLOW-Dy;y=y+2*Dy){
			Rect(x,y,x+Dx,y+Dy,12,0xFF); //canto esq cima
			Rect(x+Dx,y+Dy,x+2*Dx,y+2*Dy,12,0xFF);
		}
	Rect(0,0,255,YTOP-2,15,0);
	PutText(127-8*8,4,"[ P L A C A R ]",4);
	PutText(127 - 7*8, YLOW+2, "Esc para sair", 4);
}


void GameStart(void){
	char i;
	for(i=0;i<3;i++)
		PutText(98+i,80, "P O N G", 0);
	PutText(40,120,"Aperte qualquer tecla",0);
	// Initial Positions
	TheBall.x=128;
	TheBall.y=88;
	DirX*=-1;
	DirY*=1;
    WaitKey();
    Rect(48,4,216,13,0,0xFF);
	Cls();
}


void BallCal(struct mysprite *S){
    if (  S->x < XLEF && DirX<0)
	{LeftScore++;
	DirX=-DirX; SetBorderColor(14);SetBorderColor(0);}
	if (S->x <XLEF+8 && SpriteCollision() && DirX<0)    
		DirX=-DirX;
	//
	if (S->x >XRIG && DirX>0)
	{RightScore++;DirX=-DirX;
	SetBorderColor(0);SetBorderColor(14);}
	if (S->x >XRIG-8 && SpriteCollision() && DirX>0)
		DirX=-DirX;
	//
	if (S->y <YTOP ||  S->y >YLOW-16 )
			DirY=-DirY;
	//
	S->x+=DirX;
	S->y+=DirY;
	PutSprite( S->nn, S->pat, S->x+DirX, S->y+DirY, 0);
}

// Wait Routine
char FT_wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) 
  {
    //EnableInterupt()
    //Halt();
  }
  return(0);
}

void READjoystick(struct mysprite *S, char Jn){
	char joy;
	joy=JoystickRead(Jn);		// Reading Joystick 0 (Keyboard)
	switch(joy){
		case 1: // UP
			if (S->y>YTOP)
				S->y-=2;
			break;
		case 5:  // DOWN 		
			if (S->y<YLOW)
				S->y+=2;
			break;
	}
}
void PadCPU(struct mysprite *S,struct mysprite *R)
{
	char level=16;
    char dif=rand()%level;  //de alguma chance ao humano
	if (S->y > R->y+dif /*|| S->y>YLOW*/)
		(S->y)-=2;;
	if(S->y < R->y-dif /*|| S->y<YTOP*/)
		(S->y)+=2;
}

void Placar(){
  char mu[4];
  // Nao sei direito o que estou fazendo aqui
  // mas funciona
  mu[0] = '\0';
  _itoa(RightScore,mu,10);
  PutText(16,4,mu,4);
  mu[0]='\0';
  _itoa(LeftScore,mu,10);
  PutText(222,4,mu,4);
}

static char my_interrupt(void) {
	if (IsVsync())             // Test Vsync / Vlank
		Placar();
	return 0;
}

void main (void)
{
	char ii=128; //trocar assim que aprender interrupcao
	int delay=200;  //delay controlado pelo jogador
	struct mysprite TheBall,PadLeft,PadRight;	
	unsigned char count=0;
	// Padroes de linnas para os sprites
	char linhas[8]=  { 14,15,15,15,15,15,15,14 };
	char linhas2[8]= { 15,15,12,15,15,12,15,15 };
	char linhas3[8]= { 15, 1,15, 1,15, 1,15, 1 };
	Screen(5);
	Sprite8();
	SpriteDouble();
	KeySound(0);

	// Ball Sprite
	TheBall.nn=0;
	TheBall.x=128;
	TheBall.y=100;
	TheBall.pat=0;
	memcpy( TheBall.linecolors,linhas,sizeof(linhas));
	SetSpritePattern(TheBall.nn, PatternBall,8);
	SC5SpriteColors(TheBall.nn,linhas);
	
	// PadLeft
	PadLeft.nn=1;
	PadLeft.x=XLEF;
	PadLeft.y=100;
	PadLeft.pat=1;
	memcpy( PadLeft.linecolors,linhas2,sizeof(linhas2));
	SetSpritePattern(PadLeft.nn, PatternPad,8);
	SC5SpriteColors(PadLeft.nn,linhas3);
	PutSprite( PadLeft.nn, PadLeft.pat, PadLeft.x, PadLeft.y, 15);
	
	// PadRight
	PadRight.nn=3;
	PadRight.x=XRIG-8;
	PadRight.y=100;
	PadRight.pat=1;
	memcpy( PadRight.linecolors,linhas3,sizeof(linhas3));
	SetSpritePattern(PadRight.nn, PatternPad,8);
	SC5SpriteColors(PadRight.nn,linhas2);
	PutSprite( PadRight.nn, PadRight.pat, PadRight.x, PadRight.y, 15);
	
	DirX=1;
	DirY=1;
	
	GameStart();
	DesenhaCampo();

	while (Inkey() != 27) 				// Main loop
	{
		BallCal(&TheBall);
		PutSprite(PadLeft.nn, PadLeft.pat, PadLeft.x, PadLeft.y, 0);
		PutSprite(PadRight.nn, PadRight.pat, PadRight.x, PadRight.y, 1);
		if (ii %2) {
			READjoystick(&PadLeft, CURSORKEYS);
			PadCPU(&PadRight, &TheBall);
		}
		//
		if ( ii% 3)
			Placar();
		//FT_wait(delay);
		ii++;
	}
	// Ending program, and return to DOS
	Screen(0);
	SetColors(15,0,0);
	KeySound(1);
	Exit(0);
}