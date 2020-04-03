#ifndef __BOTAO_H__
#define __BOTAO_H__

#include "gl_canvas2d.h"

class Botao{
  float altura, largura, x, y, r, g, b;
  char label[100];

public:
  Botao(float _larg, float _alt, char *_label, bool _r, bool _g, bool _b)
  {
     altura  = _alt;
     largura = _larg;
     strcpy(label, _label);
     if(_r) r = 0.5;
     if(_g) g = 0.5;
     if(_b) b = 0.5;
  }

  void Render(bool active, int x0, int y0)
  {
      color(r + (r * active), g + (g * active), b + (b * active));
      rectFill(x0, y0 , x0 + largura, y0 + altura);

      color(0, 0, 0);
      text(x0 + 4, y0 + altura/4, label); //escreve o label do botao mais ou menos ao centro.

      x = x0;
      y = y0;
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

  int getLargura(void) {
    return largura;
  }

  int getAltura(void) {
    return altura;
  }
};

#endif
