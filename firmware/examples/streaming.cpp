#include "L3D/L3D.h"
#include <math.h>

Cube cube = Cube();

void setup()
{
  cube.begin();
}

void loop()
{
  cube.listen();
}
