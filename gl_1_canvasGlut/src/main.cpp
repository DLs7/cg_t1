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
bool f = true, r = true, b = true, g = true, m = false;

int x0, xf, y0, yf;

int offset = 32, graphOffset = 256 + (2 * offset) + 8;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

int screenWidth = 0, screenHeight = 0; //largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int minScreenHeight = 200 + (2 * offset);

int hud_x = 0;
int imgWidth = 0, imgHeight = 0;

void startButtons(int size_x, int size_y)
{
    // RGB buttons

    bf = new Botao(x0, yf + offset, size_x, size_y, (char*)"F", 1, 1, 1);
    br = new Botao(x0 + offset, yf + offset, size_x, size_y, (char*)"R", 1, 0, 0);
    bg = new Botao(x0 + (2 * offset), yf +  offset, size_x, size_y, (char*)"G", 0, 1, 0);
    bb = new Botao(x0 + (3 * offset), yf + offset, size_x, size_y, (char*)"B", 0, 0, 1);
    bl = new Botao(x0 + (4 * offset), yf + offset, size_x, size_y, (char*)"M", 1, 1, 1);

    // Scale buttons

    bplus = new Botao(x0, yf + (2 * offset), size_x, size_y, (char*)"+", 1, 1, 1);
    bminus = new Botao(x0 + offset, yf + (2 * offset), size_x, size_y, (char*)"-", 1, 1, 1);

    // Rotations buttons

    bup = new Botao(x0 + (6.5 * offset), yf + (2 * offset), size_x, size_y, (char*)"^", 1, 1, 1);
    bdown = new Botao(x0 + (6.5 * offset), yf + offset, size_x, size_y, (char*)"v", 1, 1, 1);
    bleft = new Botao(x0 + (5.5 * offset), yf + offset, size_x, size_y, (char*)"<", 1, 1, 1);
    bright = new Botao(x0 + (7.5 * offset), yf + offset, size_x, size_y, (char*)">", 1, 1, 1);
}

void renderButtons()
{
    // RGB buttons

    bf->Render(f);
    br->Render(r);
    bg->Render(g);
    bb->Render(b);
    bl->Render(m);

    // Scale buttons

    bplus->Render(true);
    bminus->Render(true);

    // Rotation buttons

    bup->Render(true);
    bdown->Render(true);
    bleft->Render(true);
    bright->Render(true);
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.
void render()
{
   clear(0, 0, 0);
   bmp->renderBitmap(offset, offset, r, g, b, rotation);

   color(0, 0, 0);
   rectFill(hud_x, 0, screenWidth, screenHeight);
   rectFill(0, screenHeight - offset, hud_x, screenHeight);
   bmp->renderHistogram(x0, y0, xf, yf, r, g, b);
   renderButtons();

   if ( opcao == 201 ) rotation = 1;
   if ( opcao == 203 ) rotation = 2;
   if ( opcao == 200 ) rotation = 3;
   if ( opcao == 202 ) rotation = 4;

   if(r || g || b) m = false;
   if(!r || !g || !b) f = false;

   if(r && g && b) f = true;
   if(!r && !g && !b) m = true;
}

void fullButton(bool x, bool y)
{
    f = x;
    r = x;
    g = x;
    b = x;
    m = y;
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   // RGB keys

   if(key == 49) fullButton(true, false);
   if(key == 50) r = !r;
   if(key == 51) g = !g;
   if(key == 52) b = !b;
   if(key == 53) fullButton(false, true);

   // Scale keys

   if(key == 61)  {
      imgWidth = imgWidth * 2;
      imgHeight = imgHeight * 2;
      bmp->resizeImage(imgWidth, imgHeight);
   }
   if(key == 45) {
      imgWidth = imgWidth / 2;
      imgHeight = imgHeight / 2;
      bmp->resizeImage(imgWidth, imgHeight);
   }
   // Rotation keys

   if(key == 201) rotation = 1;
   if(key == 203) rotation = 2;
   if(key == 200) rotation = 3;
   if(key == 202) rotation = 4;
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
}

//funcao para tratamento de mouse: cliques,  movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x;
   mouseY = y;

   if( state == 0 ) //clicou
   {
       if(bf->Colidiu(x, y)) fullButton(true, false);
       if(br->Colidiu(x, y)) r = !r;
       if(bg->Colidiu(x, y)) g = !g;
       if(bb->Colidiu(x, y)) b = !b;
       if(bl->Colidiu(x, y)) fullButton(false, true);

       if(bplus->Colidiu(x, y)) {
           imgWidth = imgWidth * 2;
           imgHeight = imgHeight * 2;
           bmp->resizeImage(imgWidth, imgHeight);
       }

       if(bminus->Colidiu(x, y)) {
           imgWidth = imgWidth / 2;
           imgHeight = imgHeight / 2;
           bmp->resizeImage(imgWidth, imgHeight);
       }

       if(bup->Colidiu(x, y)) rotation = 1;
       if(bdown->Colidiu(x, y)) rotation = 2;
       if(bleft->Colidiu(x, y)) rotation = 3;
       if(bright->Colidiu(x, y)) rotation = 4;
   }
}

void loadBmpAndSetSizes(char *path) {
   bmp = new Bmp(path);
   bmp->convertBGRtoRGB();

   imgWidth = bmp->getWidth();
   imgHeight = bmp->getHeight();

   if(imgHeight > imgWidth) {
     screenWidth = (2 * offset) + imgHeight + graphOffset;
     screenHeight = (2 * offset) + imgHeight;

     hud_x = offset + imgHeight;
   } else {
     screenWidth = (2 * offset) + imgWidth + graphOffset;
     screenHeight = (2 * offset) + imgWidth;

     hud_x = offset + imgWidth;
   }

   if(screenHeight < minScreenHeight) screenHeight = minScreenHeight;

   x0 = screenWidth + offset - graphOffset;
   xf = screenWidth - offset;

   y0 = 1.5 * offset;
   yf = 100 + 1.5 * offset;

   startButtons(offset/2, offset/2);
}

int main(void)
{
   loadBmpAndSetSizes((char*)".\\gl_1_canvasGlut\\resources\\thomas.bmp");

   initCanvas(&screenWidth, &screenHeight, "T1 - Augusto Gai Dal'Asta");
   runCanvas();
}
