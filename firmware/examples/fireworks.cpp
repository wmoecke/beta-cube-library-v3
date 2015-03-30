#include "beta-cube-library/beta-cube-library.h"
#include <math.h>

Cube cube = Cube();
int centerX, centerY, centerZ;
int launchX, launchZ;
Color rocketColor=Color(255,150,100);
int r,g,b;
int brightness=100;
float radius=0;
float speed;
bool showRocket;
bool exploded;
float xInc, yInc, zInc;
float rocketX, rocketY, rocketZ;
float launchTime;
int maxSize;


void setup()
{
  cube.begin();
  prepRocket();
}

void loop()
{
  cube.background(black);  //clear the cube for each frame
    
  if(showRocket)
    cube.shell(rocketX, rocketY, rocketZ,.05, rocketColor);

  if(exploded)
    {
      Color fireworksColor=Color(r,g,b);
        
      //make the color fade to black as the firework gets larger
      if(r>1)
	r-=2;
      if(g>1)
	g-=2;
      if(b>1)
	b-=2;
            
      cube.shell(rocketX, rocketY, rocketZ,radius, fireworksColor);
      radius+=speed;  //the sphere gets bigger
    }

  //if the rocket hasn't exploded yet, increment its position
  if(showRocket)
    {
      rocketX+=xInc;
      rocketY+=yInc;
      rocketZ+=zInc;
    }
    
  //if our firework gets too large, launch another rocket to another point
  if(radius>maxSize)
    prepRocket();
        
  if(abs(distance(centerX,centerY,centerZ,rocketX, rocketY, rocketZ)-radius)<2)
    {
      showRocket=false;
      exploded=true;
    }
    
  cube.show();  //send the data to the cube
}

float distance(float x, float y, float z, float x1, float y1, float z1)
{
  return(sqrt(pow(x-x1,2)+pow(y-y1,2)+pow(z-z1,2)));
}

void prepRocket()
{
  radius=0;
  centerX=rand()%8;
  centerY=rand()%8;
  centerZ=rand()%8;
  r=rand()%brightness;
  g=rand()%brightness;
  b=rand()%brightness;
  launchX=rand()%8;
  launchZ=rand()%8;
  rocketX=launchX;
  rocketY=0;
  rocketZ=launchZ;
  launchTime=15+rand()%25;
  xInc=(centerX-rocketX)/launchTime;
  yInc=(centerY-rocketY)/launchTime;
  zInc=(centerZ-rocketZ)/launchTime;
  showRocket=true;
  exploded=false;
  speed=0.20;
  maxSize=8;
}

