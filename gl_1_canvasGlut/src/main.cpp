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

#define BUTTON_X 16
#define BUTTON_Y 16

#define MIN_OFFSET 32

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
int rotation = 1;

bool f = true,
     r = true,
     b = true,
     g = true,
     m = false;

int x0, xf, y0, yf;

int offsetX, offsetY, graphOffset;

int screenWidth = 0, screenHeight = 0, minScreenHeight = 0; //largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int imgWidth = 0, imgHeight = 0;
int resWidth = 0, resHeight = 0;

int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;

   const HWND hDesktop = GetDesktopWindow();

   GetWindowRect(hDesktop, &desktop);

   horizontal = desktop.right;
   vertical = desktop.bottom;
}

void startButtons()
{
    // RGB buttons

    bf = new Botao(BUTTON_X, BUTTON_Y, (char*)"F", 1, 1, 1);
    br = new Botao(BUTTON_X, BUTTON_Y, (char*)"R", 1, 0, 0);
    bg = new Botao(BUTTON_X, BUTTON_Y, (char*)"G", 0, 1, 0);
    bb = new Botao(BUTTON_X, BUTTON_Y, (char*)"B", 0, 0, 1);
    bl = new Botao(BUTTON_X, BUTTON_Y, (char*)"M", 1, 1, 1);

    // Scale buttons

    bplus = new Botao(BUTTON_X, BUTTON_Y, (char*)"+", 1, 1, 1);
    bminus = new Botao(BUTTON_X, BUTTON_Y, (char*)"-", 1, 1, 1);

    // Rotations buttons

    bup = new Botao(BUTTON_X, BUTTON_Y, (char*)"^", 1, 1, 1);
    bdown = new Botao(BUTTON_X, BUTTON_Y, (char*)"v", 1, 1, 1);
    bleft = new Botao(BUTTON_X, BUTTON_Y, (char*)"<", 1, 1, 1);
    bright = new Botao(BUTTON_X, BUTTON_Y, (char*)">", 1, 1, 1);
}

void renderButtons(int x0, int y0)
{
    // RGB buttons

    bf->Render(f, x0, y0 + (2 * BUTTON_Y));
    br->Render(r, x0 + (2 * BUTTON_X), y0 + (2 * BUTTON_Y));
    bg->Render(g, x0 + (4 * BUTTON_X), y0 + (2 * BUTTON_Y));
    bb->Render(b, x0 + (6 * BUTTON_X), y0 + (2 * BUTTON_Y));
    bl->Render(m, x0 + (8 * BUTTON_X), y0 + (2 * BUTTON_Y));

    // Scale buttons

    bplus->Render(true, x0, y0 + (4 * BUTTON_Y));
    bminus->Render(true, x0 + (2 * BUTTON_X), y0 + (4 * BUTTON_Y));

    // Rotation buttons

    bup->Render(true, x0 + (13 * BUTTON_X), y0 + (4 * BUTTON_Y));
    bdown->Render(true, x0 + (13 * BUTTON_X), y0 + (2 * BUTTON_Y));
    bleft->Render(true, x0 + (11 * BUTTON_X), y0 + (2 * BUTTON_Y));
    bright->Render(true, x0 + (15 * BUTTON_X), y0 + (2 * BUTTON_Y));
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.
void render()
{
   clear(0, 0, 0);
   bmp->renderBitmap(offsetX, offsetY, imgWidth, imgHeight, r, g, b, rotation);
   bmp->renderHistogram(screenWidth - graphOffset, offsetY, screenWidth - offsetX, offsetY + 100, r, g, b);
   renderButtons(screenWidth - graphOffset, offsetY + 100);

   if(r || g || b) m = false;
   if(!r || !g || !b) f = false;

   if(r && g && b) f = true;
   if(!r && !g && !b) m = true;
}

void fullColor(bool x, bool y)
{
    f = x;
    r = x;
    g = x;
    b = x;
    m = y;
}

bool PreventCrash(float mult) {
    if(imgWidth * mult >= 1 && imgHeight * mult >= 1) return true;
    else return false;
}

bool PreventOversize(float mult) {
    if(imgWidth * mult <= resWidth && imgHeight * mult <= resHeight) return true;
    else return false;
}

void imgResize(float mult)
{
    if(PreventCrash(mult) && PreventOversize(mult)) {
        imgWidth = imgWidth * mult;
        imgHeight = imgHeight * mult;
        bmp->resizeImage(imgWidth, imgHeight);
    }

    printf("\n%dx%d", imgWidth, imgHeight);
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   // RGB keys

   if(key == 49) fullColor(true, false);
   if(key == 50) r = !r;
   if(key == 51) g = !g;
   if(key == 52) b = !b;
   if(key == 53) fullColor(false, true);

   // Scale keys

   if(key == 61) imgResize(2);
   if(key == 45) imgResize(0.5);

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
       if(bf->Colidiu(x, y)) fullColor(true, false);
       if(br->Colidiu(x, y)) r = !r;
       if(bg->Colidiu(x, y)) g = !g;
       if(bb->Colidiu(x, y)) b = !b;
       if(bl->Colidiu(x, y)) fullColor(false, true);


       if(bplus->Colidiu(x, y)) imgResize(2);
       if(bminus->Colidiu(x, y)) imgResize(0.5);

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

   GetDesktopResolution(resWidth, resHeight);

   offsetX = MIN_OFFSET; //+ screenWidth/8;
   offsetY = MIN_OFFSET; //+ screenHeight/8;

   graphOffset = 256 + offsetX;
   minScreenHeight = 200 + (2 * offsetY);

   if(imgHeight > imgWidth) {
      screenWidth = (2 * offsetX) + imgHeight + graphOffset;
      screenHeight = (2 * offsetY) + imgHeight;
   } else {
      screenWidth = (2 * offsetX) + imgWidth + graphOffset;
      screenHeight = (2 * offsetY) + imgWidth;
   }

   if(screenHeight < minScreenHeight) screenHeight = minScreenHeight;

   startButtons();
}

int main(void)
{
   loadBmpAndSetSizes((char*)".\\gl_1_canvasGlut\\resources\\thomas.bmp");

   initCanvas(&screenWidth, &screenHeight, "T1 - Augusto Gai Dal'Asta");
   runCanvas();
}
