#ifndef __BOTAO_H__
#define __BOTAO_H__

#include "gl_canvas2d.h"

class Botao{
  float altura, largura, x, y, r, g, b;
  char label[100];

public:
  Botao(float _x, float _y, float _larg, float _alt, char *_label, float _r, float _g, float _b)
  {
     altura  = _alt;
     largura = _larg;
     x = _x;
     y = _y;
     strcpy(label, _label);
     r = _r;
     g = _g;
     b = _b;
  }

  void Render()
  {
      color(r, g, b);
      rectFill(x, y, x + largura, y + altura);
      color(0, 0, 0);
      text(x + 4, y + altura/4, label); //escreve o label do botao mais ou menos ao centro.
  }

  //recebe as coordenadas do mouse para tratar clique ou detectar quando o mouse esta em cima do botao
  bool Colidiu(int mx, int my)
  {
      if( mx >= x && mx <= (x + largura) && my >= y && my <= (y + altura) )
      {
          return true;
      }
      return false;
  }

};

#endif
