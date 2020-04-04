//*********************************************************
//
// classe para fazer o carregamento de arquivos no formato BMP
// Autor: Cesar Tadeu Pozzer
//        pozzer@inf.ufsm.br
//  Versao 09/2010
//
//**********************************************************

#include "Bmp.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "gl_canvas2d.h"

Bmp::Bmp(const char *fileName)
{
   width = height = 0;
   data = NULL;

   if( fileName != NULL && strlen(fileName) > 0 ) load(fileName);
   else exit(1);

   countColors(width, height);
}

uchar* Bmp::getImage()
{
  return data;
}

int Bmp::getWidth(void)
{
  return width;
}

int Bmp::getHeight(void)
{
  return height;
}

void Bmp::convertBGRtoRGB()
{
  unsigned char tmp;
  if( data != NULL  )
  {
     for(int y=0; y<height; y++)
     for(int x=0; x<width*3; x+=3)
     {
        int pos = y*bytesPerLine + x;
        tmp = data[pos];
        data[pos] = data[pos+2];
        data[pos+2] = tmp;
     }
  }

  originalData = new unsigned char[imagesize];
  memcpy(originalData, data, imagesize);
}


void Bmp::load(const char *fileName)
{
  FILE *fp = fopen(fileName, "rb");
  if( fp == NULL )
  {
     printf("Erro ao abrir arquivo %s para leitura", fileName);
     return;
  }

  printf("Carregando arquivo %s", fileName);

  //le o HEADER componente a componente devido ao problema de alinhamento de bytes. Usando
  //o comando fread(header, sizeof(HEADER),1,fp) sao lidos 16 bytes ao inves de 14
  fread(&header.type,      sizeof(unsigned short int), 1, fp);
  fread(&header.size,      sizeof(unsigned int),       1, fp);
  fread(&header.reserved1, sizeof(unsigned short int), 1, fp);
  fread(&header.reserved2, sizeof(unsigned short int), 1, fp);
  fread(&header.offset,    sizeof(unsigned int),       1, fp);

  //le o INFOHEADER componente a componente devido ao problema de alinhamento de bytes
  fread(&info.size,        sizeof(unsigned int),       1, fp);
  fread(&info.width,       sizeof(int),                1, fp);
  fread(&info.height,      sizeof(int),                1, fp);
  fread(&info.planes,      sizeof(unsigned short int), 1, fp);
  fread(&info.bits,        sizeof(unsigned short int), 1, fp);
  fread(&info.compression, sizeof(unsigned int),       1, fp);
  fread(&info.imagesize,   sizeof(unsigned int),       1, fp);
  fread(&info.xresolution, sizeof(int),                1, fp);
  fread(&info.yresolution, sizeof(int),                1, fp);
  fread(&info.ncolours,    sizeof(unsigned int),       1, fp);
  fread(&info.impcolours,  sizeof(unsigned int),       1, fp);

  width  = info.width;
  height = info.height;
  bits   = info.bits;
  bytesPerLine = (3 * (width + 1) / 4) * 4;
  imagesize    = bytesPerLine*height;
  int delta    = bytesPerLine - (3 * width);

  printf("\nImagem: %dx%d - Bits: %d", width, height, bits);
  printf("\nbytesPerLine: %d", bytesPerLine);
  printf("\nbytesPerLine: %d", width * 3);
  printf("\ndelta: %d", delta);
  printf("\nimagesize: %d %d", imagesize, info.imagesize);
  printf("\n\n");

  if( header.type != 19778 )
  {
     printf("\nError: Arquivo BMP invalido");
     getchar();
     exit(0);
  }

  if( width*height*3 != imagesize )
  {
     printf("\nWarning: Arquivo BMP nao eh potencia de 2");
     getchar();
  }

  if( info.compression != 0 )
  {
     printf("\nError: Formato BMP comprimido nao suportado");
     getchar();
     return;
  }
  if( bits != 24 )
  {
     printf("\nError: Formato BMP com %d bits/pixel nao suportado", bits);
     getchar();
     return;
  }

  if( info.planes != 1 )
  {
     printf("\nError: Numero de Planes nao suportado: %d", info.planes);
     getchar();
     return;
  }

  data = new unsigned char[imagesize];
  fread(data, sizeof(unsigned char), imagesize, fp);

  fclose(fp);
}

void Bmp::startCount()
{
    r_maxCount = 0;
    g_maxCount = 0;
    b_maxCount = 0;
    l_maxCount = 0;

    for(int i = 0; i < 256; i++) {
        r_count[i] = 0;
        g_count[i] = 0;
        b_count[i] = 0;
        l_count[i] = 0;
    }
}

int Bmp::max3(int a, int b, int c) {
    if (a >= b && a >= c) return a;
    else if (b >= a && b >= c) return b;
    else if (c >= a && c >= b) return c;
}

void Bmp::auxCount(int maxCount, int v_max, int *count, int i) {
    if(count[i] > maxCount) {
        maxCount = count[i];
        v_max = i;
    }
}

void Bmp::getMaxCount(int *this_count, int i, int *maxCount, int *this_max)
{
    if(this_count[i] > *maxCount) {
        *maxCount = this_count[i];
        *this_max = i;
    }
}

void Bmp::countColors(int h, int w)
{
    startCount();

    int sum = 0;

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            float red = (float)data[sum];
            float green = (float)data[sum + 1];
            float blue = (float)data[sum + 2];
            float lum = (red * 0.299) + (green * 0.587) + (blue * 0.114);

            r_count[(int)round(red)]++;
            g_count[(int)round(green)]++;
            b_count[(int)round(blue)]++;
            l_count[(int)round(lum)]++;

            sum += 3;
        }
    }

    for(int i = 0; i < 256; i++) {
        getMaxCount(r_count, i, &r_maxCount, &r_max);
        getMaxCount(g_count, i, &g_maxCount, &g_max);
        getMaxCount(b_count, i, &b_maxCount, &b_max);
        getMaxCount(l_count, i, &l_maxCount, &l_max);
    }


    c_max = max3(r_maxCount, g_maxCount, b_maxCount);
}

void Bmp::resizeImage(int new_x, int new_y)
{
    float scale_x = (float)new_x/(float)width;
    float scale_y = (float)new_y/(float)height;

    int bpl = (3 * (new_x + 1)/4) * 4;
    int imgsize = bpl * new_y;

    data = new unsigned char [imgsize];

    for (int y = 0; y < new_y; y++) {
        for (int x = 0; x < new_x; x++) {
            int new_pos = (y * bpl) + (x * 3);
            int old_pos = floor(y/scale_y) * bytesPerLine + floor(x/scale_x) * 3;

            data[new_pos] = originalData[old_pos];
            data[new_pos + 1] = originalData[old_pos + 1];
            data[new_pos + 2] = originalData[old_pos + 2];
        }
    }

    countColors(new_x, new_y);
}

void Bmp::renderBitmap(int pos_x, int pos_y, int w, int h, bool r, bool g, bool b, int rotation)
{
    clear(0, 0, 0);

    int sum = 0;
    float red, green, blue, lum;

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            if(!r && !g && !b) {
                red = ((float)data[sum]/255) * 0.299;
                green = ((float)data[sum + 1]/255) * 0.587;
                blue = ((float)data[sum + 2]/255) * 0.114;

                lum = red + green + blue;

                color(lum, lum, lum);
            } else {
                red = ((float)data[sum]/255) * r;
                green = ((float)data[sum + 1]/255) * g;
                blue = ((float)data[sum + 2]/255) * b;

                color(red, green, blue);
            }

            if (rotation == 1) {
                point(pos_x + x, pos_y + y);
            } else if (rotation == 2) {
                point(pos_x + w - x, pos_y + h - y);
            } else if (rotation == 3) {
                point(pos_x + h - y, pos_y + x);
            } else if (rotation == 4) {
                point(pos_x + y, pos_y + w - x);
            }

            sum += 3;
        }
    }
}

void Bmp::renderHistogramPoint(int x0, int y0, int *c_count, int x, bool t) {
    int pos = (int)round((c_count[x] * 100)/c_max);
    if(t == true) point(x0 + x, y0 + pos);
}

void Bmp::renderHistogram(int x0, int y0, int xf, int yf, bool r, bool g, bool b)
{
    graph(x0, y0, xf, yf);
    drawMaxGraph(x0, y0, r, g, b);

    for(int x = 0; x < 256; x++) {
        color(1, 0, 0);
        renderHistogramPoint(x0, y0, r_count, x, r);

        color(0, 1, 0);
        renderHistogramPoint(x0, y0, g_count, x, g);

        color(0, 0, 1);
        renderHistogramPoint(x0, y0, b_count, x, b);

        color(1, 1, 1);
        renderHistogramPoint(x0, y0, l_count, x, (!r && !g && !b));
    }
}

void Bmp::setMaxGraph(int x0, int y0, int this_max, int this_maxCount, int this_c_max, bool t) {
    if(t) {
        int xi = x0 - 5, xf = x0 + 5;
        int yi = y0 - 5, yf = y0 + 5;

        int x = x0 + this_max;
        int y = y0 + (int)round((this_maxCount * 100)/c_max);

        line(xi, y, xf, y);
        line(x, yi, x, yf);
    }
}

void Bmp::drawMaxGraph(int x0, int y0, bool r, bool g, bool b)
{
    color(1, 0, 0);
    setMaxGraph(x0, y0, r_max, r_maxCount, c_max, r);

    color(0, 1, 0);
    setMaxGraph(x0, y0, g_max, g_maxCount, c_max, g);

    color(0, 0, 1);
    setMaxGraph(x0, y0, b_max, b_maxCount, c_max, b);

    color(1, 1, 1);
    setMaxGraph(x0, y0, l_max, l_maxCount, l_maxCount, (!r && !g && !b));
}

int Bmp::getMax(void) {
    return c_max;
}

int Bmp::graphTextOffset(int value) {
    if (value/100 < 1)
        return 25;
    else if (value/100 < 10)
        return 35;
    else if (value/ 100 < 100)
        return 45;
    else if (value/ 100 < 1000)
        return 55;
    else if (value/100 < 10000)
        return 65;
}

void Bmp::graph(int x0, int y0, int xf, int yf)
{
    color(1, 1, 1);

    int med = c_max/2;
    char b[7];

    // EIXO X

    text(x0 - 4, y0 - 16, "0");
    text(x0 + ((xf - x0)/2) - 14, y0 - 16, "127");
    text(xf - 14, y0 - 16, "255");
    for(int x = 0; x <= 256; x += 16) {
        line(x0 + x, y0 - 2, x0 + x, y0 + 2);
    }

    // SETA EIXO X

    line(x0, y0, xf + 8, y0);
    line(xf + 4, y0 + 4, xf + 8, y0);
    line(xf + 4, y0 - 4, xf + 8, y0);

    // EIXO Y

    text(x0 - 15, y0 - 4, "0");

    sprintf(b, "%d", med);
    text(x0 - graphTextOffset(med), y0 + ((yf - y0)/2) - 4, b);

    sprintf(b, "%d", c_max);
    text(x0 - graphTextOffset(c_max), yf - 4, b);

    for(int y = 0; y <= 100; y+=10) {
        line(x0 - 2, y0 + y, x0 + 2, y0 + y);
    }

    // SETA EIXO Y

    line(x0, y0, x0, yf + 8);
    line(x0 - 4, yf + 4, x0, yf + 8);
    line(x0 + 4, yf + 4, x0, yf + 8);
}
