/***************************************************************************************
// TRABALHO A - AUGUSTO GAI DAL'ASTA (MATR. 201810291)
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
// > Suporte para separação individual de canais de cor.
// > Botões de cor mostram se estão ativos ou inativos;
// > Janela escalável (se a imagem estiver muito grande, é só aumentar a janela para ver
//   ela por completo, sem detrimento à HUD do programa);
// > Escalamento de imagem por nearest neighbor.
// > Possibilidade de "reset" de imagem para o original.
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
#include <GL/freeglut_ext.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gl_canvas2d.h"

#include "Botao.h"
#include "Bmp.h"

// Tamanho dos botões
#define BUTTON_X 16
#define BUTTON_Y 16

// Offset padrão
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

// A escala, por default, é setada para cima.
int rotation = 1;

// O RGB, por default, é ligado.
bool f = true,
     r = true,
     b = true,
     g = true,
     m = false;

// Offsets do programa.
// Apesar de serem iguais, resolvi fazer offsetX e offsetY separados para ficar mais fácil de refatorar futuramente.
int offsetX, offsetY, graphOffset;

// Variáveis de largura e altura da tela e da imagem,
// e de altura mínima da tela para que a HUD continue sendo renderizada por completo.
int screenWidth = 0, screenHeight = 0, minScreenHeight = 0;
int imgWidth = 0, imgHeight = 0;

int mouseX, mouseY;

// Essa função cria os botões.
// Ela recebe o tamanho, o título do botão, e a cor que eu quero passar para eles.
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

// Variável que renderiza os botões.
// Ela também atualiza o booleano f e m, que indicam se a imagem está Full RGB ou Monocromática.
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

// Esta função renderiza a HUD: botões e histograma.
void renderHud() {
   int hudX = screenWidth - graphOffset + offsetX;

   // Essa função serve para desenhar um retângulo preto atrás da HUD, para que, quando
   // a gente escalar uma imagem para cima, essa imagem não apareça embaixo da HUD.
   color(0, 0, 0);
   if(m) rectFill(hudX - bmp->graphTextOffset(bmp->getMaxLum()), 0, screenWidth, screenHeight);
   else  rectFill(hudX - bmp->graphTextOffset(bmp->getMax()), 0, screenWidth, screenHeight);

   // Renderiza o histograma e os botões aqui. Recebe os booleanos r, g, b para ligar ou
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

// Chama a função de resize da imagem e atualiza o tamanho atual dela.
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

    // Na classe Bmp, eu armazeno as informações da data original, assim como a altura e a largura originais.
    imgWidth = bmp->getWidth();
    imgHeight = bmp->getHeight();

    bmp->restoreData();
}

// Função feita pra encurtar o código ao ligar Full RGB ou Monocromática.
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

// Função chamada toda vez que uma tecla for pressionada.
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

// Função chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
}

// Função para tratamento de mouse: cliques,  movimentos e arrastos
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

// Carrega o BMP e ajusta o tamanho da tela. Único propósito de existência dessa função
// é não deixar a main feia.
void loadBmpAndSetSizes(char *path) {
   bmp = new Bmp(path);
   bmp->convertBGRtoRGB();

   imgWidth = bmp->getWidth();
   imgHeight = bmp->getHeight();

   offsetX = MIN_OFFSET;
   offsetY = MIN_OFFSET;

   graphOffset = 256 + (2 * offsetX);
   minScreenHeight = 200 + offsetY;

   // Caso a height > width, faço o screenWidth e o screenHeight ser baseado na height da imagem.
   // Serve para que, quando uma pessoa for rotacionar a tela logo de primeira, a imagem não fique
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
   // mínimo definido anteriormente.
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
