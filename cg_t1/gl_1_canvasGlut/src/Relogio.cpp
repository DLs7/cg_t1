#include "Relogio.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define POS_X 230
#define POS_Y 300

Relogio::Relogio()
{
   hora = 0;
}

void Relogio::anima()
{
   clear(0,0,0);
   color(1,0,0);

   int x = (int)(cos(hora) * 100);
   int y = (int)(sin(hora) * 100);
   //clear(0, 1, 0);
   //color(0,0,0);
   circle(POS_X, POS_Y, 110, 30);
   line(POS_X, POS_Y, POS_X + x, POS_Y + y);
   hora += 0.001;
}

