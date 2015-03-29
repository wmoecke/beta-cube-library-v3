#include "L3D/L3D.h"

Cube cube = Cube();

void setup()
{
  cube.begin();
  cube.show();
}

void loop()
{
  cube.listen();
}
