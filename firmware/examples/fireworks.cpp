int centerX, centerY, centerZ;
int launchX, launchZ;
int brightness=100;
float radius=0;
float speed;
bool showRocket;
bool exploded;
float xInc, yInc, zInc;
float rocketX, rocketY, rocketZ;
float launchTime;
int maxSize;
Color fireworksColor;

void setup()
{
    cube.begin();
    prepRocket();
}

void loop(){
  //set all the pixels to black to start with
  cube.background(black);
    

  //loop through all the pixels, calculate the distance to the center point, and turn the pixel on if it's at the right radius
  for(int x=0;x<cube.side;x++)
    for(int y=0;y<cube.side;y++) 
      for(int z=0;z<cube.side;z++)
        {
	  if(showRocket)
	    cube.shell(x,y,z,radius,0.05,Color{255,150,100});
	  if(exploded)
	    cube.shell(x,y,z,radius,0.1,Color{red, green,});
	    if(abs(distance(x,y,z,centerX, centerY, centerZ)-radius)<0.1)
	      setPixel(x,y,z,red, green, blue);
        }

  if(exploded)
    radius+=speed;  //the sphere gets bigger

  if(showRocket)
    {
      rocketX+=xInc;
      rocketY+=yInc;
      rocketZ+=zInc;
    }
  //if our sphere gets too large, restart the animation in another random spot
  if(radius>maxSize)
    prepRocket();
  if(abs(distance(centerX,centerY,centerZ,rocketX, rocketY, rocketZ)-radius)<2)
    {
      showRocket=false;
      exploded=true;
    }

  cube.show();  //refresh the cube
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
  fireworksColor={rand()%brightness,rand()%brightness,rand()%brightness};  //pick random R,G,B values for the color
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
