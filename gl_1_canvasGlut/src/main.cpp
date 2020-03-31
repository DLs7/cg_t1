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

#include "Botao.h"
#include "Bmp.h"

Bmp *bmp = NULL;

Botao *bf = NULL;
Botao *br = NULL;
Botao *bg = NULL;
Botao *bb = NULL;
Botao *bl = NULL;

Botao *bup = NULL;
Botao *bdown = NULL;
Botao *bleft = NULL;
Botao *bright = NULL;

Botao *bplus = NULL;
Botao *bminus = NULL;

//variavel global para selecao do que sera exibido na canvas.
int opcao  = 49;
int auxOpcao = opcao;
int rotation = 1;

int screenWidth = 0, screenHeight = 0; //largura e altura inicial da tela . Alteram com o redimensionamento de tela.

int x0, xf, y0, yf;

int imgOffset, offset = 32, graphOffset = 256 + (2 * offset) + 8;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

void startButtons(int size_x, int size_y) {
    bf = new Botao(x0, yf + offset, size_x, size_y, "F", 1, 1, 1);
    br = new Botao(x0 + offset, yf + offset, size_x, size_y, "R", 1, 0, 0);
    bg = new Botao(x0 + (2 * offset), yf +  offset, size_x, size_y, "G", 0, 1, 0);
    bb = new Botao(x0 + (3 * offset), yf + offset, size_x, size_y, "B", 0, 0, 1);
    bl = new Botao(x0 + (4 * offset), yf + offset, size_x, size_y, "M", 0.5, 0.5, 0.5);

    bplus = new Botao(x0, yf + (2 * offset), size_x, size_y, "+", 1, 1, 1);
    bminus = new Botao(x0 + offset, yf + (2 * offset), size_x, size_y, "-", 1, 1, 1);

    bup = new Botao(x0 + (6.5 * offset), yf + (2 * offset), size_x, size_y, "^", 1, 1, 1);
    bdown = new Botao(x0 + (6.5 * offset), yf + offset, size_x, size_y, "v", 1, 1, 1);
    bleft = new Botao(x0 + (5.5 * offset), yf + offset, size_x, size_y, "<", 1, 1, 1);
    bright = new Botao(x0 + (7.5 * offset), yf + offset, size_x, size_y, ">", 1, 1, 1);
}

void renderButtons() {
    bf->Render();
    br->Render();
    bg->Render();
    bb->Render();
    bl->Render();

    bplus->Render();
    bminus->Render();

    bup->Render();
    bdown->Render();
    bleft->Render();
    bright->Render();
}

void fullRender()
{
    bmp->renderBitmap(offset, imgOffset, 1, 1, 1, rotation);
    bmp->renderHistogram(x0, y0, xf, yf, 1, 1, 1);
}

void redRender()
{
    bmp->renderBitmap(offset, imgOffset, 1, 0, 0, rotation);
    bmp->renderHistogram(x0, y0, xf, yf, 1, 0, 0);
}

void greenRender()
{
    bmp->renderBitmap(offset, imgOffset, 0, 1, 0, rotation);
    bmp->renderHistogram(x0, y0, xf, yf, 0, 1, 0);
}

void blueRender()
{
    bmp->renderBitmap(offset, imgOffset, 0, 0, 1, rotation);
    bmp->renderHistogram(x0, y0, xf, yf, 0, 0, 1);
}

void monoRender()
{
    bmp->renderBitmapMonochrome(offset, imgOffset, rotation);
    bmp->renderHistogramMonochrome(x0, y0, xf, yf);
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.
void render()
{
   renderButtons();

   if ( opcao == 201 ) rotation = 1;
   else if ( opcao == 203 ) rotation = 2;
   else if ( opcao == 200 ) rotation = 3;
   else if ( opcao == 202 ) rotation = 4;

   if ( opcao == 49 )
   {
      fullRender();
      auxOpcao = opcao;
   }
   else if( opcao == 50 ) //50 -> vermelho
   {
      redRender();
      auxOpcao = opcao;
   }
   else if( opcao == 51 ) //'3' -> verde
   {
      greenRender();
      auxOpcao = opcao;
   }
   else if( opcao == 52 ) //'4' -> azul
   {
      blueRender();
      auxOpcao = opcao;
   }
   else if ( opcao == 53 ) {
      monoRender();
      auxOpcao = opcao;
   }
   else
   {
      opcao = auxOpcao;

      if(opcao == 49) fullRender();
      else if(opcao == 50) redRender();
      else if(opcao == 51) greenRender();
      else if(opcao == 52) blueRender();
      else if(opcao == 53) monoRender();
   }
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   printf("\nTecla: %d" , key);
   if( key < 204 )
   {
      opcao = key;
   }
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   printf("\nLiberou: %d" , key);
}

//funcao para tratamento de mouse: cliques,  movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x;
   mouseY = y;

   if( state == 0 ) //clicou
   {
       if(bf->Colidiu(x, y)) opcao = 49;
       if(br->Colidiu(x, y)) opcao = 50;
       if(bg->Colidiu(x, y)) opcao = 51;
       if(bb->Colidiu(x, y)) opcao = 52;
       if(bl->Colidiu(x, y)) opcao = 53;
       if(bup->Colidiu(x, y)) rotation = 1;
       if(bdown->Colidiu(x, y)) rotation = 2;
       if(bleft->Colidiu(x, y)) rotation = 3;
       if(bright->Colidiu(x, y)) rotation = 4;
   }

   //printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);
}

int main(void)
{
   bmp = new Bmp(".\\gl_1_canvasGlut\\resources\\landscape.bmp");
   bmp->convertBGRtoRGB();

   screenWidth = (2 * offset) + bmp->getWidth() + graphOffset;
   screenHeight = (2 * offset) + bmp->getHeight();

   imgOffset = screenHeight - bmp->getHeight() - offset;

   x0 = screenWidth + offset - graphOffset;
   y0 = offset + offset/2;
   xf = screenWidth - offset;
   yf = 100 + offset + offset/2;

   startButtons(offset/2, offset/2);

   initCanvas(&screenWidth, &screenHeight, "BMP - Pressione 1, 2, 3, 4 ou 5");

   runCanvas();
}
