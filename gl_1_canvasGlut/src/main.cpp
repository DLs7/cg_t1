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

//variavel global para selecao do que sera exibido na canvas.
int opcao  = 49;
int auxOpcao = opcao;

int screenWidth = 0, screenHeight = 0; //largura e altura inicial da tela . Alteram com o redimensionamento de tela.

int graph_x0, graph_xf, graph_y0, graph_yf;

int imgOffset, offset = 32, graphOffset = 256 + (2 * offset);
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

void DrawMouseScreenCoords()
{
    char str[100];
    sprintf(str, "Mouse: (%d,%d)", mouseX, mouseY);
    text(10,300, str);
    sprintf(str, "Screen: (%d,%d)", screenWidth, screenHeight);
    text(10,320, str);
}

void fullRender()
{
    bmp->renderBitmap(offset, imgOffset, 1, 1, 1);
    bmp->renderHistogram(graph_x0, graph_y0, graph_xf, graph_yf, 1, 1, 1);
}

void redRender()
{
    bmp->renderBitmap(offset, imgOffset, 1, 0, 0);
    bmp->renderHistogram(graph_x0, graph_y0, graph_xf, graph_yf, 1, 0, 0);
}

void greenRender()
{
    bmp->renderBitmap(offset, imgOffset, 0, 1, 0);
    bmp->renderHistogram(graph_x0, graph_y0, graph_xf, graph_yf, 0, 1, 0);
}

void blueRender()
{
    bmp->renderBitmap(offset, imgOffset, 0, 0, 1);
    bmp->renderHistogram(graph_x0, graph_y0, graph_xf, graph_yf, 0, 0, 1);
}

void monoRender()
{
    bmp->renderBitmapMonochrome(offset, imgOffset);
    bmp->renderHistogramMonochrome(graph_x0, graph_y0, graph_xf, graph_yf);
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

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;

   printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);
}

int main(void)
{
   bmp = new Bmp(".\\gl_1_canvasGlut\\resources\\img1.bmp");
   //bmp = new Bmp(".\\gl_1_canvasGlut\\resources\\img2.bmp");
   //bmp = new Bmp(".\\gl_1_canvasGlut\\resources\\img3.bmp");
   //bmp = new Bmp(".\\gl_1_canvasGlut\\resources\\img4.bmp");
   bmp->convertBGRtoRGB();

   screenWidth = (2 * offset) + bmp->getWidth() + graphOffset;
   screenHeight = (2 * offset) + bmp->getHeight();

   imgOffset = screenHeight - bmp->getHeight() - offset;

   if(screenHeight < 100 + 2*(offset) + offset/2){
      screenHeight = 100 + 2*(offset) + offset/2;
   }

   graph_x0 = screenWidth + offset - graphOffset;
   graph_y0 = offset + offset/2;
   graph_xf = screenWidth - offset;
   graph_yf = 100 + offset + offset/2;

   initCanvas(&screenWidth, &screenHeight, "BMP - Pressione 1, 2, 3, 4 ou 5");

   runCanvas();
}
