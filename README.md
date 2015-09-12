Particle L3D Cube Library
======================

## About

This is a branch of enjrolas' official L3D library for use with the Particle Photon in the [L3D Cube](http://l3dcube.com).

It provides 3D drawing functions for writing visual effects and utility functions for working with color. The library also takes care of communication with the LED strips making up the cube.

This branch has been created with the main purpose of adding text capability to the official L3D library - this has been accomplished by porting Hape's TextLayer library for the Processing (java-based) language. (BTW, thanks a lot, Hape.)

The API can be found in ../firmware/beta-cube-library-v3.h. Further examples can be found in ../firmware/examples and some documentation on the [L3D Cube website](http://docs.l3dcube.com/).

## Example Usage

```C++
// Define the paging start address of the font table
const int COMPUTER_FONT=0;
const int CUBE_FONT=256; 

//Cube cube = Cube(8, 50, CUBE_FONT); // Explicitly define cube size, max brightness and font table start address
Cube cube = Cube(); // Defaults to 8x8x8 cube with max brightness of 50 and the start address of 256 (CUBE_FONT)
cube.begin(); // initialize the cube
cube.background(Color(0, 0, 0)); // clear the cube

std::string message="L3D Cube!";
// Scroll some text across the sides of the cube
cube.marquee(message, pos, cube.colorMap(frameCount%1000, 0, 1000));
// Scroll some text from right to left, across the center of the cube
cube.scrollText(message, Point(pos - (message.length() + 1), 0, 4), cube.colorMap(random(8, 256), 0, 255));
// Scroll some spinning text from right to left, across the center of the cube
cube.scrollSpinningText(message, Point(pos - (message.length() + 1), 0, 3), cube.colorMap(random(8, 256), 0, 255));
// Show a spinning character in the center of the cube
cube.showChar('D', Point(3.5, 3.5, 3.5), Point(3.5, 3.5, 3.5), Point(0, pos, 0), cube.colorMap(random(8, 256), 0, 255));

cube.show(); // Update the LEDs to make the changes visible
```

## Building Locally

To compile a firmware binary using a local copy of the library:

1. Follow the instructions for compiling the [Spark firmware](https://github.com/spark/firmware#1-download-and-install-dependencies). Make sure you can successfuly compile the firmware before continuing.
2. Edit the FIRMWARE_DIR variable in the l3d-cube makefile to the path of the spark firmware repository on your machine.
3. Choose an example to compile or put your own code in firmware/examples.
4. Run `make bin/<name of example>.bin` to generate firmware for that example in the bin/ directory. For example, to compile examples/bit-of-everything.cpp run `make bin/bit-of-everything.bin`.
5. Flash the firmware using `spark flash` (the [Spark CLI tool](https://github.com/spark/spark-cli)) or dfu-util.
