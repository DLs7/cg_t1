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
#include "gl_canvas2d.h"

Bmp::Bmp(const char *fileName)
{
   width = height = 0;
   data = NULL;

   if( fileName != NULL && strlen(fileName) > 0 )
   {
      load(fileName);
      printf("\nFile opened.");
   }
   else
   {
      printf("Error: Invalid BMP filename");
   }

   startCount();
   countColors();
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
  if( data != NULL )
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
}


void Bmp::load(const char *fileName)
{
  FILE *fp = fopen(fileName, "rb");
  if( fp == NULL )
  {
     printf("\nErro ao abrir arquivo %s para leitura", fileName);
     return;
  }

  printf("\n\nCarregando arquivo %s", fileName);

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
  bytesPerLine =(3 * (width + 1) / 4) * 4;
  imagesize    = bytesPerLine*height;
  int delta    = bytesPerLine - (3 * width);

  printf("\nImagem: %dx%d - Bits: %d", width, height, bits);
  printf("\nbytesPerLine: %d", bytesPerLine);
  printf("\nbytesPerLine: %d", width * 3);
  printf("\ndelta: %d", delta);
  printf("\nimagesize: %d %d", imagesize, info.imagesize);

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
    r_max = 0;
    g_max = 0;
    b_max = 0;
    l_max = 0;

    for(int i = 0; i < 256; i++) {
        r_count[i] = 0;
        g_count[i] = 0;
        b_count[i] = 0;
        l_count[i] = 0;
    }
}

void Bmp::renderBitmap(int pos_x, int pos_y, bool r, bool g, bool b, int rotation)
{
    clear(0,0,0);

    int sum = 0;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(!r && !g && !b) {
                float red = ((float)data[sum]/255) * 0.299;
                float green = ((float)data[sum + 1]/255) * 0.587;
                float blue = ((float)data[sum + 2]/255) * 0.114;

                float lum = red + green + blue;

                color(lum, lum, lum);
            } else {
                float red = ((float)data[sum]/255) * r;
                float green = ((float)data[sum + 1]/255) * g;
                float blue = ((float)data[sum + 2]/255) * b;

                color(red, green, blue);
            }

            if (rotation == 1) {
                point(pos_x + x, pos_y + y);
            } else if (rotation == 2) {
                point(pos_x + width - x, pos_y + height - y);
            } else if (rotation == 3) {
                point(pos_x + height - y, pos_y + x);
            } else if (rotation == 4) {
                point(pos_x + y, pos_y + width - x);
            }

            sum += 3;
        }
    }
}

void Bmp::renderHistogram(int x0, int y0, int xf, int yf, bool r, bool g, bool b)
{
    graph(x0, y0, xf, yf);

    if(!r && !g && !b) {
        for(int x = 0; x < 256; x++) {
            color(1, 1, 1);
            int l_pos = (int)round((l_count[x] * 100)/l_max);
            point(x0 + x, y0 + l_pos);
        }
    } else {
        for(int x = 0; x < 256; x++) {
            color(1, 0, 0);
            int r_pos = (int)round((r_count[x] * 100)/r_max);
            if(r == true) point(x0 + x, y0 + r_pos);

            color(0, 1, 0);
            int g_pos = (int)round((g_count[x] * 100)/g_max);
            if(g == true) point(x0 + x, y0 + g_pos);

            color(0, 0, 1);
            int b_pos = (int)round((b_count[x] * 100)/b_max);
            if(b == true) point(x0 + x, y0 + b_pos);
        }
    }
}

int Bmp::max4(int a, int b, int c, int d) {
    int e = a > b ? a : b;
    int f = c > d ? c : d;
    return e > f ? e : f;
}

void Bmp::graph(int x0, int y0, int xf, int yf)
{
    color(1, 1, 1);

    char b[3];

    int max = max4(r_max, g_max, b_max, l_max);
    int med = max/2;

    text(x0 - 4, y0 - 16, "0");
    text(x0 + ((xf - x0)/2) - 18, y0 - 16, "127");
    text(xf - 22, y0 - 16, "255");
    for(int x = 0; x <= 256; x += 16) {
        line(x0 + x, y0 - 2, x0 + x, y0 + 2);
    }

    line(x0, y0, xf + 8, y0);
    line(xf + 4, y0 + 4, xf + 8, y0);
    line(xf + 4, y0 - 4, xf + 8, y0);

    text(x0 - 16, y0 - 4, "0");

    sprintf(b, "%d", med);
    text(x0 - 36, y0 + ((yf - y0)/2) - 4, b);

    sprintf(b, "%d", max);
    text(x0 - 36, yf - 4, b);

    for(int y = 0; y <= 100; y+=10) {
        line(x0 - 2, y0 + y, x0 + 2, y0 + y);
    }

    line(x0, y0, x0, yf + 8);
    line(x0 - 4, yf + 4, x0, yf + 8);
    line(x0 + 4, yf + 4, x0, yf + 8);
}

void Bmp::countColors()
{
    int sum = 0;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            float red = (float)data[sum];
            float green = (float)data[sum + 1];
            float blue = (float)data[sum + 2];
            float lum = (red * 0.299) + (green * 0.587) + (blue * 0.114);

            r_count[(int)red]++;
            g_count[(int)green]++;
            b_count[(int)blue]++;
            l_count[(int)lum]++;

            sum += 3;
        }
    }

    for(int i = 0; i < 256; i++) {
        if(r_count[i] > r_max) r_max = r_count[i];
        if(g_count[i] > g_max) g_max = g_count[i];
        if(b_count[i] > b_max) b_max = b_count[i];
        if(l_count[i] > l_max) l_max = l_count[i];
    }
}
