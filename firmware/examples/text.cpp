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
    cube.background(black);
    switch(mode) {
        case(0):    
            cube.showChar('L', Point(3.5, 0, 3.5), Point(0, pos, 0), cube.colorMap(frameCount%1000, 0, 1000));
            break;  
        case(1):
            cube.showChar('3', Point(1, 3.5, 3.5), Point(0, 3.5, 0), Point(pos, 0, 0), cube.colorMap(frameCount%1000, 0, 1000));
            break;  
        case(2):
            cube.showChar('D', Point(1.5, 0, 3.5), Point(0, 0, 0), Point(0, -pos, 0), cube.colorMap(frameCount%1000, 0, 1000));
            break;  
        case(3):
            message="L3D Cube!";
            cube.marquee(message, pos, cube.colorMap(frameCount%1000, 0, 1000));
            break;  
        case(4):
            message="cubism";
            cube.scrollText(message, Point(pos, 0, 4), cube.colorMap(frameCount%1000, 0, 1000));
            break;  
        case(5):
            message="1^3";
            cube.scrollSpinningText(message, Point(pos, 0, 3), cube.colorMap(frameCount%1000, 0, 1000));
            break;
    }
    frameCount++;
    pos += posInc;
    if (pos > (message.length() + 1) * cube.size) {
        pos = 0;
        mode++;
        if(mode > 5)
            mode = 0;
    }
}