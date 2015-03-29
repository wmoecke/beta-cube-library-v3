#include "L3D/L3D.h"
#include <math.h>

Cube cube = Cube();
float t=0;
float radius=4;
int offset;
Point corners[4];
Point end[4];
int frame=0;
Point beginning=Point(2,3.5,3.5);
void setup()
{
  cube.begin();
  corners[0]=Point(cube.size-1, 0, 0);
  corners[1]=Point(cube.size-1, cube.size-1, 0);
  corners[2]=Point(cube.size-1, cube.size-1, cube.size-1);
  corners[3]=Point(cube.size-1, 0, cube.size-1);
}

void loop()
{
  cube.background(black);
  float theta;    
  for(int i=0;i<4;i++)
    {
      switch(i){
      case(0):
	theta=M_PI/2;
	break;
      case(1):
	theta=0;
	break;
      case(2):
	theta=3*M_PI/2;
	break;
      case(3):
	theta=M_PI;
	break;
      }
      end[i]=Point(corners[i].x, corners[i].y + radius*sin(t+theta), corners[i].z + radius*cos(t+theta));
    }
  for(int i=0;i<4;i+=2)
    cube.line(beginning,end[(i+offset)%4], cube.colorMap((frame+125*i)%500,0,500));
  t-=.05;
  if(t<-M_PI/2)
    {
      t=0;
      offset--;
      if(offset<0)
        offset=3;
      delay(10);
    }
  frame++;
  cube.show();
}
