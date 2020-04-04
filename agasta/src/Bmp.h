//*********************************************************
//
// classe para fazer o carregamento de arquivos no formato BMP
// Autor: Cesar Tadeu Pozzer
//        pozzer@inf.ufsm.br
//
//  Referencia:  http://astronomy.swin.edu.au/~pbourke/dataformats/bmp/
//  Versao 09/2010
//
//**********************************************************

#ifndef ___BMP__H___
#define ___BMP__H___

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gl_canvas2d.h"


#define HEADER_SIZE      14 //sizeof(HEADER) vai dar 16 devido ao alinhamento de bytes
#define INFOHEADER_SIZE  40 //sizeof(INFOHEADER) da 40 e esta correto.
#define uchar unsigned char

typedef struct {
   unsigned short int type;                 /* Magic identifier            */
   unsigned int size;                       /* File size in bytes          */
   unsigned short int reserved1, reserved2;
   unsigned int offset;                     /* Offset to image data em bytes*/
} HEADER;


typedef struct {
   unsigned int size;               /* Header size in bytes      */
   int width,height;                /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int impcolours;         /* Important colours         */
} INFOHEADER;


class Bmp
{
private:
   int width, height,                  // Largura e altura da imagem
       imagesize,                      // Tamanho, em bytes, da imagem
       bytesPerLine, bits,             // Quantidade de bytes por linha e bits totais da imagem
       r_max, r_maxCount,              // Tom vermelho mais predominante e quantas vezes ele se repetiu
       g_max, g_maxCount,              // Tom verde mais predominante e quantas vezes ele se repetiu
       b_max, b_maxCount,              // Tom azul mais predominante e quantas vezes ele se repetiu
       l_max, l_maxCount, c_maxCount;  // Valor de luminância mais predominante e quantas vezes ele se repetiu
   int r_count[256],                   // Vetor responsável por contar cada tom de vermelho.
       g_count[256],                   // Vetor responsável por contar cada tom de verde.
       b_count[256],                   // Vetor responsável por contar cada tom de azul.
       l_count[256];                   // Vetor responsável por contar cada valor de luminância.
   unsigned char *data, *originalData;

   HEADER     header;
   INFOHEADER info;

   void load(const char *fileName);
   void startCount();
   void countColors(int w, int h);
   void graph(int x0, int y0, int xf, int yf, bool r, bool g, bool b);
   void drawMaxGraph(int x0, int y0, bool r, bool g, bool b);
   void getMaxCount(int *this_count, int i, int *maxCount, int *this_max);
   void renderHistogramPoint(int x0, int y0, int *c_count, int this_maxCount, int x, bool t);
   void setMaxGraph(int x0, int y0, int this_max, int this_maxCount, int this_c_max, bool t);
   void placeText(int this_maxCount, int x0, int y0, int yf);
   int  max3(int a, int b, int c);

public:
   Bmp(const char *fileName);
   uchar* getImage();
   int    getWidth(void);
   int    getHeight(void);
   int    getMax(void);
   int    getMaxLum(void);
   int    graphTextOffset(int value);
   void   convertBGRtoRGB(void);
   void   renderBitmap(int pos_x, int pos_y, int w, int h, bool r, bool g, bool b, int rotation);
   void   renderHistogram(int x0, int y0, int xf, int yf, bool r, bool g, bool b);
   void   resizeImage(int new_x, int new_y);
   void   restoreData(void);
};

#endif
