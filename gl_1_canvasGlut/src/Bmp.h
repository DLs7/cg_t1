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
   int width, height, imagesize, bytesPerLine, bits, r_max, r_maxCount, g_max, g_maxCount, b_max, b_maxCount, l_max, l_maxCount, max;
   int r_count[256], g_count[256], b_count[256], l_count[256];
   unsigned char *data;

   HEADER     header;
   INFOHEADER info;

   void load(const char *fileName);
   void startCount();
   void countColors();
   void countLum();
   void graph(int x0, int y0, int xf, int yf);
   void drawMaxGraph(int x0, int y0, bool r, bool g, bool b);
   int max3(int a, int b, int c);

public:
   Bmp(const char *fileName);
   uchar* getImage();
   int    getWidth(void);
   int    getHeight(void);
   void   convertBGRtoRGB(void);
   void   renderBitmap(int pos_x, int pos_y, bool r, bool g, bool b, int rotation);
   void   renderHistogram(int x0, int y0, int xf, int yf, bool r, bool g, bool b);
};

#endif
