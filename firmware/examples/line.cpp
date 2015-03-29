#include "L3D/L3D.h"

Cube cube = Cube();
int pos;
int inc=1;
int frame=0;
void setup()
{
  cube.begin();
  cube.background(black);
}

void loop()
{
  cube.background(black);
  Point start={pos, cube.size-1,0}; //x,y,z values of the high, rear point
  Point end={cube.size-1-pos,0,cube.size-1};  //x,y,z values of the low front point
  cube.line(start, end, cube.colorMap(frame%1000,0,1000));
  pos+=inc;

  if((pos==0)||(pos==cube.size-1))  //if the points go beyond the bounds of the cube, bounce them back the other way
    inc*=-1;

  frame++; //this just keeps track of how many frames we've shown.  I'm using it to change the line color over time
  cube.show();
}
