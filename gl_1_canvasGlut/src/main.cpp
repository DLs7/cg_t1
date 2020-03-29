/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Cesar Tadeu Pozzer
//         03/2020
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp
//
//  Instruções:
//	  Para alterar a animacao, digite numeros entre 1 e 3
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gl_canvas2d.h"

#include "Bola.h"
#include "Relogio.h"
#include "Botao.h"
#include "Bmp.h"


Bola    *b = NULL;
Relogio *r = NULL;
Botao   *bt = NULL; //se a aplicacao tiver varios botoes, sugiro implementar um manager de botoes.
Bmp     *bmp = NULL;

//variavel global para selecao do que sera exibido na canvas.
int opcao  = 49;
int auxOpcao = opcao;
int screenWidth = 1024, screenHeight = 512; //largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int imgOffset, offset = 32;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

void RenderBitmap(unsigned char *data, int pos_x, int pos_y, bool r, bool g, bool b)
{
    clear(0,0,0);

    int sum = 0;

    for(int y = 0; y < bmp->getHeight(); y++) {
        for(int x = 0; x < bmp->getWidth(); x++) {
            float red = ((float)data[sum]/255) * r;
            float green = ((float)data[sum + 1]/255) * g;
            float blue = ((float)data[sum + 2]/255) * b;

            color(red, green, blue);
            point(pos_x + x, pos_y + y);

            sum += 3;
        }
    }
}

void RenderMonochromeBitmap(unsigned char *data, int pos_x, int pos_y)
{
    clear(0,0,0);

    int sum = 0;

    for(int y = 0; y < bmp->getHeight(); y++) {
        for(int x = 0; x < bmp->getWidth(); x++) {
            float red = ((float)data[sum]/255) * 0.299;
            float green = ((float)data[sum + 1]/255) * 0.587;
            float blue = ((float)data[sum + 2]/255) * 0.144;

            float lum = red + green + blue;

            color(lum, lum, lum);
            point(pos_x + x, pos_y + y);

            sum += 3;
        }
    }
}

void DrawMouseScreenCoords()
{
    char str[100];
    sprintf(str, "Mouse: (%d,%d)", mouseX, mouseY);
    text(10,300, str);
    sprintf(str, "Screen: (%d,%d)", screenWidth, screenHeight);
    text(10,320, str);
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.
void render()
{
   //text(20,500,"Programa Demo Canvas2D");

   //DrawMouseScreenCoords();

   //bt->Render();

   //DesenhaLinhaDegrade();

   if ( opcao == 49 )
   {
      RenderBitmap(bmp->getImage(), offset, imgOffset, 1, 1, 1);
      auxOpcao = opcao;
   }
   else if( opcao == 50 ) //50 -> vermelho
   {
      RenderBitmap(bmp->getImage(), offset, imgOffset, 1, 0, 0);
      auxOpcao = opcao;
   }
   else if( opcao == 51 ) //'3' -> verde
   {
      RenderBitmap(bmp->getImage(), offset, imgOffset, 0, 1, 0);
      auxOpcao = opcao;
   }
   else if( opcao == 52 ) //'4' -> azul
   {
      RenderBitmap(bmp->getImage(), offset, imgOffset, 0, 0 ,1);
      auxOpcao = opcao;
   }
   else if ( opcao == 53 ) {
      RenderMonochromeBitmap(bmp->getImage(), offset, imgOffset);
      auxOpcao = opcao;
   }
   else
   {
      opcao = auxOpcao;

      if(opcao == 49)      RenderBitmap(bmp->getImage(), offset, imgOffset, 1, 1, 1);
      else if(opcao == 50) RenderBitmap(bmp->getImage(), offset, imgOffset, 1, 0, 0);
      else if(opcao == 51) RenderBitmap(bmp->getImage(), offset, imgOffset, 0, 1, 0);
      else if(opcao == 52) RenderBitmap(bmp->getImage(), offset, imgOffset, 0, 0 ,1);
      else if(opcao == 53) RenderMonochromeBitmap(bmp->getImage(), offset, imgOffset);
   }
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   printf("\nTecla: %d" , key);
   if( key < 200 )
   {
      opcao = key;
   }

   switch(key)
   {
      case 27:
	     exit(0);
	  break;

	  //seta para a esquerda
      case 200:
         b->move(-10);
	  break;

	  //seta para a direita
	  case 202:
         b->move(10);
	  break;
   }
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   printf("\nLiberou: %d" , key);
}

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;

   //printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);

   if( state == 0 ) //clicou
   {
       if( bt->Colidiu(x, y) )
       {
           printf("\nClicou no botao\n");
       }
   }
}

int main(void)
{
   initCanvas(&screenWidth, &screenHeight, "Titulo da Janela: Canvas 2D - Pressione 1, 2, 3");

   b = new Bola();
   r = new Relogio();
   bt = new Botao(200, 400, 140, 50, "Sou um botao");
   bmp = new Bmp(".\\gl_1_canvasGlut\\img\\img.bmp");

   imgOffset = screenHeight - bmp->getHeight() - offset;

   bmp->convertBGRtoRGB();

   runCanvas();
}
