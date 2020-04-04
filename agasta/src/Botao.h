#ifndef __BOTAO_H__
#define __BOTAO_H__

#include "gl_canvas2d.h"

class Botao{
  // Armazena a altura e a largura do bot�o, assim como booleanos que identificam a cor do mesmo.
  // Tamb�m armazena o seu label.
  float altura, largura, x, y, r, g, b;
  char label[6];

public:
  // Cria um bot�o dado uma largura, uma altura, um label e booleanos r, g, b que definem a cor.
  // A posi��o do bot�o n�o � dada, uma vez que a posi��o dele constantemente atualiza.
  // Essa posi��o � passada no m�todo Render.
  Botao(float _larg, float _alt, char *_label, bool _r, bool _g, bool _b)
  {
     altura  = _alt;
     largura = _larg;
     strcpy(label, _label);
     if(_r) r = 0.5;
     if(_g) g = 0.5;
     if(_b) b = 0.5;
  }

  // Renderiza o bot�o.
  // O active intensifica a cor dele caso seja true (ou seja, ele est� ativado), e o x0, y0
  // definem a posi��o deles baseada no tamanho da tela.
  void Render(bool active, int x0, int y0)
  {
      color(r + (r * active), g + (g * active), b + (b * active));
      rectFill(x0, y0 , x0 + largura, y0 + altura);

      color(0, 0, 0);
      text(x0 + 4, y0 + altura/4, label); //escreve o label do botao mais ou menos ao centro.

      // Guardamos o x e o y para o m�todo Colidiu funcionar.
      x = x0;
      y = y0;
  }

  // Recebe as coordenadas do mouse para tratar clique ou detectar quando o mouse est� em cima do bot�o.
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
