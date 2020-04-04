/***************************************************************************************
// TRABALHO A - AUGUSTO GAI DAL'ASTA (MATR. 201810291)
//
//
// Este trabalho conta com as seguintes funcionalidades essenciais:
// > Carregamento de imagem colorida (RGB) em formato BMP;
// > Exibição dessa imagem de várias formas: Original, com remoção individual de canais
//   RGB e em tons de cinza (luminância);
// > Rotação de imagem no eixo vertical e horizontal;
// > Plotagem do histograma de cada canal RGB e da luminância;
// > Escalamento de imagem, podendo diminuí-la ou aumentá-la pela metade.
//
// Este trabalho também conta com as seguintes funcionalidades adicionais:
// > Suporte tanto para botões na tela quanto para inputs de teclado;
// > Botões de cor mostram se estão ativos ou inativos;
// > Janela escalável (se a imagem estiver muito grande, é só aumentar a janela para ver
//   ela por completo, sem detrimento à HUD do programa);
// > Escalamento de imagem por nearest neighbor.
//
// INSTRUÇÕES:
// > PARA MUDAR A COR DA IMAGEM:
//   >> Botão F ou tecla "1" -> Ativa os canais RGB na imagem e no histograma;
//   >> Botão R ou tecla "2" -> Ativa/desativa o canal vermelho na imagem e no histograma;
//   >> Botão G ou tecla "3" -> Ativa/desativa o canal verde na imagem e no histograma;
//   >> Botão B ou tecla "4" -> Ativa/desativa o canal azul na imagem e no histograma;
//   >> Botão M ou tecla "5" -> Desativa os canais RGB na imagem e no histograma, e mostra
//      a imagem em tons de cinza (luminância).
//
// > PARA ROTACIONAR A IMAGEM:
//   >> Botão ^ ou seta para cima -> Rotaciona a imagem na vert. para cima;
//   >> Botão v ou seta para baixo -> Rotaciona a imagem na vert. para baixo;
//   >> Botão < ou seta para esquerda -> Rotaciona a imagem na horiz. para a esquerda;
//   >> Botão > ou seta para direita -> Rotaciona a imagem na horiz. para a direita.
//
// > PARA AUMENTAR OU DIMINUIR A ESCALA DA IMAGEM:
//   >> Botão + ou tecla "+" -> Aumenta a imagem em 2x;
//   >> Botão - ou tecla "-" -> Diminui a imagem em 1/2;
//
// > PARA RESTAURAR A IMAGEM AO PADRÃO:
//   >> Botão "Reset" ou tecla "esc".
//
***************************************************************************************/

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

Botao *breset = NULL;

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

int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

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

    // Reset image button

    breset = new Botao(BUTTON_X * 5, BUTTON_Y, (char*)" Reset", 1, 1, 1);
}

void renderButtons(int x0, int y0)
{
    // RGB buttons

    if(r || g || b) m = false;
    if(!r || !g || !b) f = false;

    if(r && g && b) f = true;
    if(!r && !g && !b) m = true;

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

    // Reset imagem button

    breset->Render(true, x0 + (4 * BUTTON_X), y0 + (4 * BUTTON_Y));
}

void renderHud() {
   int hudX = screenWidth - graphOffset + offsetX;

   color(0, 0, 0);
   rectFill(hudX - bmp->graphTextOffset(bmp->getMax()), 0, screenWidth, screenHeight);

   bmp->renderHistogram(hudX, 1.5 * offsetY, screenWidth - offsetX, 1.5 * offsetY + 100, r, g, b);
   renderButtons(hudX, 1.5 * offsetY + 100);
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.
void render()
{
   clear(0, 0, 0);
   bmp->renderBitmap(offsetX, offsetY, imgWidth, imgHeight, r, g, b, rotation);

   renderHud();
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

void imgResize(float mult)
{
    if(PreventCrash(mult)) {
        imgWidth = imgWidth * mult;
        imgHeight = imgHeight * mult;
        bmp->resizeImage(imgWidth, imgHeight);
    }
}

void resetImage() {
    f = true;
    r = true;
    g = true;
    b = true;
    m = false;

    imgWidth = bmp->getWidth();
    imgHeight = bmp->getHeight();

    bmp->restoreData();
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   if(key == 27) resetImage();

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
       if(breset->Colidiu(x, y)) resetImage();

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

   offsetX = MIN_OFFSET;
   offsetY = MIN_OFFSET;

   graphOffset = 256 + (2 * offsetX);
   minScreenHeight = 200;

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
   loadBmpAndSetSizes((char*)".\\agasta\\resources\\img1.bmp");

   initCanvas(&screenWidth, &screenHeight, "T1 - Augusto Gai Dal'Asta");
   runCanvas();
}
