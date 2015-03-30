#include "beta-cube-library/beta-cube-library.h"

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
