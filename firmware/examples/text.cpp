#include "beta-cube-library.h"

// Define the paging start address of the font table
const int COMPUTER_FONT=0;
const int CUBE_FONT=256;    

Cube cube = Cube(8, 50, CUBE_FONT);
//Cube cube = Cube();

float pos = 0;
float posInc = .1;
int mode = 0, frameCount = 0;
std::string message = " ";


void transition(Color bgcolor, float speed);
bool transitioning(Color bgcolor);


void setup() {
    cube.begin();
    cube.background(black);
    cube.show();
}

void loop() {
    drawCube();
    cube.show();
}


void drawCube() {
    if(!cube.done) {
        cube.background(black);
        switch(mode) {
            case(0):    
                cube.showChar('L', Point(3.5, 0, 3.5), Point(0, pos, 0), cube.colorMap(frameCount%1000, 0, 1000));
                break;  
            case(1):
                cube.showChar('3', Point(1, 3.5, 3.5), Point(0, 3.5, 0), Point(pos, 0, 0), cube.colorMap(frameCount%1000, 0, 1000));
                break;  
            case(2):
                cube.showChar('D', Point(3.5, 3.5, 3.5), Point(3.5, 3.5, 3.5), Point(0, pos, 0), cube.colorMap(frameCount%1000, 0, 1000));
                break;  
            case(3):
                message="L3D Cube!";
                cube.marquee(message, pos, cube.colorMap(frameCount%1000, 0, 1000));
                break;  
            case(4):
                message="cubism";
                cube.scrollText(message, Point(pos - (message.length() + 1), 0, 4), cube.colorMap(frameCount%1000, 0, 1000));
                break;  
            case(5):
                message="1^3";
                cube.scrollSpinningText(message, Point(pos - (message.length() + 1), 0, 3), cube.colorMap(frameCount%1000, 0, 1000));
                break;
        }
        frameCount++;
        pos += posInc;
        if (pos > (message.length() + 1) * cube.size) {
            pos = 0;
            mode++;
            cube.done = true;
            if(mode > 5)
                mode = 0;
        }
    }
    else
        transition(black, posInc);
}

void transition(Color bgcolor, float speed) {
    cube.done = transitioning(bgcolor);
    if(!cube.done) return;
    
    for(int x = 0; x < cube.size; x++)
        for(int y = cube.size-1; y >= 0; y--)
            for(int z = 0; z < cube.size; z++) {
                Color pixelColor = cube.getVoxel(x, y, z);
                if(pixelColor != (Color)black) {
                    if(pixelColor.red > 0)
                        pixelColor.red = constrain(pixelColor.red - speed, 0, cube.maxBrightness);
                    if(pixelColor.green > 0)
                        pixelColor.green = constrain(pixelColor.green - speed, 0, cube.maxBrightness);
                    if(pixelColor.blue > 0)
                        pixelColor.blue = constrain(pixelColor.blue - speed, 0, cube.maxBrightness);
                    cube.setVoxel(x, y, z, pixelColor);
                }
            }    
}

bool transitioning(Color bgcolor) {
    for(int x = 0; x < cube.size; x++)
        for(int y = 0; y < cube.size; y++)
            for(int z = 0; z < cube.size; z++) {
                Color pixelColor = cube.getVoxel(x, y, z);
                if(pixelColor != bgcolor) return true;
            }
    
    return false;
}