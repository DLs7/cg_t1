/***************************************************************************************
// TRABALHO A - AUGUSTO GAI DAL'ASTA (MATR. 201810291)
//
// Este trabalho conta com as seguintes funcionalidades essenciais:
// > Carregamento de imagem colorida (RGB) em formato BMP;
// > Exibi��o dessa imagem de v�rias formas: Original, com remo��o individual de canais
//   RGB e em tons de cinza (lumin�ncia);
// > Rota��o de imagem no eixo vertical e horizontal;
// > Plotagem do histograma de cada canal RGB e da lumin�ncia;
// > Escalamento de imagem, podendo diminu�-la ou aument�-la pela metade.
//
// Este trabalho tamb�m conta com as seguintes funcionalidades adicionais:
// > Suporte tanto para bot�es na tela quanto para inputs de teclado;
// > Suporte para separa��o individual de canais de cor.
// > Bot�es de cor mostram se est�o ativos ou inativos;
// > Janela escal�vel (se a imagem estiver muito grande, � s� aumentar a janela para ver
//   ela por completo, sem detrimento � HUD do programa);
// > Escalamento de imagem por nearest neighbor.
// > Possibilidade de "reset" de imagem para o original.
//
// INSTRU��ES:
// > PARA MUDAR A COR DA IMAGEM:
//   >> Bot�o F ou tecla "1" -> Ativa os canais RGB na imagem e no histograma;
//   >> Bot�o R ou tecla "2" -> Ativa/desativa o canal vermelho na imagem e no histograma;
//   >> Bot�o G ou tecla "3" -> Ativa/desativa o canal verde na imagem e no histograma;
//   >> Bot�o B ou tecla "4" -> Ativa/desativa o canal azul na imagem e no histograma;
//   >> Bot�o M ou tecla "5" -> Desativa os canais RGB na imagem e no histograma, e mostra
//      a imagem em tons de cinza (lumin�ncia).
//
// > PARA ROTACIONAR A IMAGEM:
//   >> Bot�o ^ ou seta para cima -> Rotaciona a imagem na vert. para cima;
//   >> Bot�o v ou seta para baixo -> Rotaciona a imagem na vert. para baixo;
//   >> Bot�o < ou seta para esquerda -> Rotaciona a imagem na horiz. para a esquerda;
//   >> Bot�o > ou seta para direita -> Rotaciona a imagem na horiz. para a direita.
//
// > PARA AUMENTAR OU DIMINUIR A ESCALA DA IMAGEM:
//   >> Bot�o + ou tecla "+" -> Aumenta a imagem em 2x;
//   >> Bot�o - ou tecla "-" -> Diminui a imagem em 1/2;
//
// > PARA RESTAURAR A IMAGEM AO PADR�O:
//   >> Bot�o "Reset" ou tecla "esc".
//
***************************************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gl_canvas2d.h"

#include "Botao.h"
#include "Bmp.h"

// Tamanho dos bot�es
#define BUTTON_X 16
#define BUTTON_Y 16

// Offset padr�o
#define MIN_OFFSET 32

// Bitmap
Bmp *bmp = NULL;

// RGB buttons
Botao *bf = NULL;
Botao *br = NULL;
Botao *bg = NULL;
Botao *bb = NULL;
Botao *bl = NULL;

// Rotation buttons
Botao *bup = NULL;
Botao *bdown = NULL;
Botao *bleft = NULL;
Botao *bright = NULL;

// Scale buttons
Botao *bplus = NULL;
Botao *bminus = NULL;

// Reset button
Botao *breset = NULL;

// A escala, por default, � setada para cima.
int rotation = 1;

// O RGB, por default, � ligado.
bool f = true,
     r = true,
     b = true,
     g = true,
     m = false;

// Offsets do programa.
// Apesar de serem iguais, resolvi fazer offsetX e offsetY separados para ficar mais f�cil de refatorar futuramente.
int offsetX, offsetY, graphOffset;

// Vari�veis de largura e altura da tela e da imagem,
// e de altura m�nima da tela para que a HUD continue sendo renderizada por completo.
int screenWidth = 0, screenHeight = 0, minScreenHeight = 0;
int imgWidth = 0, imgHeight = 0;

int mouseX, mouseY;

// Essa fun��o cria os bot�es.
// Ela recebe o tamanho, o t�tulo do bot�o, e a cor que eu quero passar para eles.
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

// Vari�vel que renderiza os bot�es.
// Ela tamb�m atualiza o booleano f e m, que indicam se a imagem est� Full RGB ou Monocrom�tica.
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

    // Reset image button
    breset->Render(true, x0 + (4 * BUTTON_X), y0 + (4 * BUTTON_Y));
}

// Esta fun��o renderiza a HUD: bot�es e histograma.
void renderHud() {
   int hudX = screenWidth - graphOffset + offsetX;

   // Essa fun��o serve para desenhar um ret�ngulo preto atr�s da HUD, para que, quando
   // a gente escalar uma imagem para cima, essa imagem n�o apare�a embaixo da HUD.
   color(0, 0, 0);
   if(m) rectFill(hudX - bmp->graphTextOffset(bmp->getMaxLum()), 0, screenWidth, screenHeight);
   else  rectFill(hudX - bmp->graphTextOffset(bmp->getMax()), 0, screenWidth, screenHeight);

   // Renderiza o histograma e os bot�es aqui. Recebe os booleanos r, g, b para ligar ou
   // desligar esses canais no histograma.
   bmp->renderHistogram(hudX, 1.5 * offsetY, screenWidth - offsetX, 1.5 * offsetY + 100, r, g, b);
   renderButtons(hudX, 1.5 * offsetY + 100);
}

// Render.
void render()
{
   bmp->renderBitmap(offsetX, offsetY, imgWidth, imgHeight, r, g, b, rotation);
   renderHud();
}

// Previne que a imagem assuma um valor menor que 1x1
bool PreventCrash(float mult) {
    if(imgWidth * mult >= 1 && imgHeight * mult >= 1) return true;
    else return false;
}

// Chama a fun��o de resize da imagem e atualiza o tamanho atual dela.
void imgResize(float mult)
{
    // Caso o resultado desse resize seja maior que uma imagem 1x1.
    if(PreventCrash(mult)) {
        imgWidth = imgWidth * mult;
        imgHeight = imgHeight * mult;
        bmp->resizeImage(imgWidth, imgHeight);
    }
}

// Reseta a imagem para o estado original.
void resetImage() {

    f = true;
    r = true;
    g = true;
    b = true;
    m = false;

    rotation = 1;

    // Na classe Bmp, eu armazeno as informa��es da data original, assim como a altura e a largura originais.
    imgWidth = bmp->getWidth();
    imgHeight = bmp->getHeight();

    bmp->restoreData();
}

// Fun��o feita pra encurtar o c�digo ao ligar Full RGB ou Monocrom�tica.
// Se f = true, r = g = b = true, m = false
// Se m = true, r = g = b = false, f = false
void fullColor(bool x, bool y)
{
    f = x;
    r = x;
    g = x;
    b = x;
    m = y;
}

// Fun��o chamada toda vez que uma tecla for pressionada.
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

// Fun��o chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
}

// Fun��o para tratamento de mouse: cliques,  movimentos e arrastos
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

// Carrega o BMP e ajusta o tamanho da tela. �nico prop�sito de exist�ncia dessa fun��o
// � n�o deixar a main feia.
void loadBmpAndSetSizes(char *path) {
   bmp = new Bmp(path);
   bmp->convertBGRtoRGB();

   imgWidth = bmp->getWidth();
   imgHeight = bmp->getHeight();

   offsetX = MIN_OFFSET;
   offsetY = MIN_OFFSET;

   graphOffset = 256 + (2 * offsetX);
   minScreenHeight = 200 + offsetY;

   // Caso a height > width, fa�o o screenWidth e o screenHeight ser baseado na height da imagem.
   // Serve para que, quando uma pessoa for rotacionar a tela logo de primeira, a imagem n�o fique
   // abaixo da HUD.
   if(imgHeight > imgWidth) {
      screenWidth = (2 * offsetX) + imgHeight + graphOffset;
      screenHeight = (2 * offsetY) + imgHeight;
   // Vice-versa caso a width > height
   } else {
      screenWidth = (2 * offsetX) + imgWidth + graphOffset;
      screenHeight = (2 * offsetY) + imgWidth;
   }

   // Se o screenHeight definido for menor que o tamanho da HUD, seto a height da tela para um tamanho
   // m�nimo definido anteriormente.
   if(screenHeight < minScreenHeight) screenHeight = minScreenHeight;

   startButtons();
}

int main(void)
{
   // img1.bmp = imagem normal_1 da demo de textura
   // img3.bmp = exemplo de imagem com width > height
   // img4.bmp = exemplo de imagem com height > widht
   loadBmpAndSetSizes((char*)".\\agasta\\resources\\img1.bmp");

   initCanvas(&screenWidth, &screenHeight, "TA - Augusto Gai Dal'Asta");
   runCanvas();
}
