#include <math.h>
#include "beta-cube-library-v3.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

/** Construct a new cube.
  @param s Size of one side of the cube in number of LEDs.
  @param mb Maximum brightness value. Used to prevent the LEDs from drawing too much current (which causes the colors to distort).

  @return A new Cube object.
  */
Cube::Cube(unsigned int s, unsigned int mb, unsigned int ft) : \
    maxBrightness(mb),
    onlinePressed(false),
    lastOnline(true),
    strip(Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE)),
    size(s),
	// initialize Text members
    done(false),
    looping(false),
    selectedFont((ft==CUBE_FONT || ft==COMPUTER_FONT) 
				? ft : CUBE_FONT)
{ }

/** Construct a new cube with default settings.
  @param s Size of one side of the cube in number of LEDs.
  @param mb Maximum brightness value. Used to prevent the LEDs from drawing too much current (which causes the colors to distort).

  @return A new Cube object.
  */
Cube::Cube() : \
    maxBrightness(50),
    onlinePressed(true),
    lastOnline(true), 
    strip(Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE)),
    size(8),
	// initialize Text members
    done(false),
    looping(false),
    selectedFont(CUBE_FONT)
{ }

/** Initialization of cube resources and environment. */
void Cube::begin(void) {
  this->strip.begin();
  this->center=Point((size-1)/2,(size-1)/2,(size-1)/2);  
  // initialize Spark variables
  int (Cube::*setPort)(String) = &Cube::setPort;

  Particle.variable("IPAddress", this->localIP, STRING);
  Particle.variable("MACAddress", this->macAddress, STRING);
  Particle.variable("port", &this->port, INT);
  Particle.function("setPort", (int (*)(String)) setPort);

  this->initButtons();
  this->udp.begin(STREAMING_PORT);
  this->updateNetworkInfo();
  Particle.connect();
}

/** Overloaded != operator. */
bool operator != (const Color& b) const {
    if(*this.red != b.red) return true;
    if(*this.green != b.green) return true;
    if(*this.blue != b.blue) return true;
    return false;
}

/** Overloaded == operator. */
bool operator == (const Color& b) const {
    if(*this.red != b.red) return false;
    if(*this.green != b.green) return false;
    if(*this.blue != b.blue) return false;
    return true;
}

/** Set a voxel at a position to a color.

  @param x, y, z Coordinate of the LED to set.
  @param col Color to set the LED to.
  */
void Cube::setVoxel(int x, int y, int z, Color col)
{
  if(x >= 0 && y >= 0 && z >= 0 &&
      x < this->size && y < this->size && z < this->size) {
    int index = (z*64) + (x*8) + y;
    this->strip.setPixelColor(index, this->strip.Color(col.red, col.green, col.blue));
  }
}

/** Set a voxel at a position to a color.

  @param p Coordinate of the LED to set.
  @param col Color to set the LED to.
  */
void Cube::setVoxel(Point p, Color col)
{
  this->setVoxel(p.x, p.y, p.z, col);
}

/** Get the color of a voxel at a position.

  @param x, y, z Coordinate of the LED to get the color from.
  */
Color Cube::getVoxel(int x, int y, int z)
{
  int index = (z * this->size * this->size) + (x * this->size) + y;
  uint32_t col = this->strip.getPixelColor(index);
  Color pixelColor = Color((col>>16) & 0xff, (col>>8) & 0xff, col & 0xff);
  return pixelColor;
}

/** Get the color of a voxel at a position.

  @param p Coordinate of the LED to get the color from.
  */
Color Cube::getVoxel(Point p)
{
  return this->getVoxel(p.x, p.y, p.z);
}

/** Draw a line in 3D space.
  Uses the 3D form of Bresenham's algorithm.

  @param x1, y1, z1 Coordinate of start of line.
  @param x2, y2, z2 Coordinate of end of line.
  @param col Color of the line.
  */
void Cube::line(int x1, int y1, int z1, int x2, int y2, int z2, Color col)
{
  Point currentPoint = Point(x1, y1, z1);

  int dx = x2 - x1;
  int dy = y2 - y1;
  int dz = z2 - z1;
  int x_inc = (dx < 0) ? -1 : 1;
  int l = abs(dx);
  int y_inc = (dy < 0) ? -1 : 1;
  int m = abs(dy);
  int z_inc = (dz < 0) ? -1 : 1;
  int n = abs(dz);
  int dx2 = l << 1;
  int dy2 = m << 1;
  int dz2 = n << 1;

  if((l >= m) && (l >= n)) {
    int err_1 = dy2 - l;
    int err_2 = dz2 - l;

    for(int i = 0; i < l; i++) {
      this->setVoxel(currentPoint, col);

      if(err_1 > 0) {
        currentPoint.y += y_inc;
        err_1 -= dx2;
      }

      if(err_2 > 0) {
        currentPoint.z += z_inc;
        err_2 -= dx2;
      }

      err_1 += dy2;
      err_2 += dz2;
      currentPoint.x += x_inc;
    }
  } else if((m >= l) && (m >= n)) {
    int err_1 = dx2 - m;
    int err_2 = dz2 - m;

    for(int i = 0; i < m; i++) {
      this->setVoxel(currentPoint, col);

      if(err_1 > 0) {
        currentPoint.x += x_inc;
        err_1 -= dy2;
      }

      if(err_2 > 0) {
        currentPoint.z += z_inc;
        err_2 -= dy2;
      }

      err_1 += dx2;
      err_2 += dz2;
      currentPoint.y += y_inc;
    }
  } else {
    int err_1 = dy2 - n;
    int err_2 = dx2 - n;

    for(int i = 0; i < n; i++) {
      this->setVoxel(currentPoint, col);

      if(err_1 > 0) {
        currentPoint.y += y_inc;
        err_1 -= dz2;
      }

      if(err_2 > 0) {
        currentPoint.x += x_inc;
        err_2 -= dz2;
      }

      err_1 += dy2;
      err_2 += dx2;
      currentPoint.z += z_inc;
    }
  }

  this->setVoxel(currentPoint, col);
}

/** Draw a line in 3D space.
  Uses the 3D form of Bresenham's algorithm.

  @param p1 Coordinate of start of line.
  @param p2 Coordinate of end of line.
  @param col Color of the line.
  */
void Cube::line(Point p1, Point p2, Color col)
{
  this->line(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, col);
}

/** Draw a filled sphere.

  @param x, y, z Position of the center of the sphere.
  @param r Radius of the sphere.
  @param col Color of the sphere.
  */
void Cube::sphere(int x, int y, int z, int r, Color col)
{
  for(int dx = -r; dx <= r; dx++) {
    for(int dy = -r; dy <= r; dy++) {
      for(int dz = -r; dz <= r; dz++) {
        if(sqrt(dx*dx + dy*dy + dz*dz) <= r) {
          this->setVoxel(x + dx, y + dy, z + dz, col);
        }
      }
    }
  }
}

/** Draw a filled sphere.

  @param p Position of the center of the sphere.
  @param r Radius of the sphere.
  @param col Color of the sphere.
  */
void Cube::sphere(Point p, int r, Color col)
{
  this->sphere(p.x, p.y, p.z, r, col);
}

/** Draw a shell (empty sphere).

  @param x Position of the center of the shell.
  @param y Position of the center of the shell.
  @param z Position of the center of the shell.
  @param r Radius of the shell.
  @param col Color of the shell.
*/
void Cube::shell(float x, float y,float z, float r, Color col)
{
  float thickness =0.1;
  for(int i=0;i<this->size;i++)
    for(int j=0;j<this->size;j++)
      for(int k=0;k<this->size;k++)
    	if(abs(sqrt(pow(i-x,2)+pow(j-y,2)+pow(k-z,2))-r)<thickness)
    	  this->setVoxel(i,j,k,col);
}

/** Draw a shell (empty sphere).

  @param x Position of the center of the shell.
  @param y Position of the center of the shell.
  @param z Position of the center of the shell.
  @param r Radius of the shell.
  @param thickness Thickness of the shell.
  @param col Color of the shell.
*/
void Cube::shell(float x, float y,float z, float r, float thickness, Color col)
{
  for(int i=0;i<this->size;i++)
    for(int j=0;j<this->size;j++)
      for(int k=0;k<this->size;k++)
    	if(abs(sqrt(pow(i-x,2)+pow(j-y,2)+pow(k-z,2))-r)<thickness)
    	  this->setVoxel(i,j,k,col);
}

/** Draw a shell (empty sphere).

  @param p Position of the center of the shell.
  @param r Radius of the shell.
  @param col Color of the shell.
*/
void Cube::shell(Point p, float r, Color col)
{
  this->shell(p.x, p.y, p.z, r, col);
}

/** Draw a shell (empty sphere).

  @param p Position of the center of the shell.
  @param r Radius of the shell.
  @param thickness Thickness of the shell
  @param col Color of the shell.
*/
void Cube::shell(Point p, float r, float thickness, Color col)
{
  this->shell(p.x, p.y, p.z, r, thickness, col);
}

/** Draw an empty circle in the XZ plane.
  Uses the midpoint circle algorithm.

  @param p Position of the center of the circle.
  @param r Radius of the circle.
  @param col Color of the circle.
*/
void Cube::emptyFlatCircle(int x, int y, int z, int r, Color col)
{
  int dx = r;
  int dy = 0;
  int radiusError = 1 - dx;

  while(dx >= dy) {
    this->setVoxel(x + dx,  y, z + dy, col);
    this->setVoxel(x + dy,  y, z + dx, col);
    this->setVoxel(x + -dx, y, z + dy, col);
    this->setVoxel(x + -dy, y, z + dx, col);
    this->setVoxel(x + -dx, y, z + -dy, col);
    this->setVoxel(x + -dy, y, z + -dx, col);
    this->setVoxel(x + dx,  y, z + -dy, col);
    this->setVoxel(x + dy,  y, z + -dx, col);

    dy++;

    if(radiusError<0) {
      radiusError += 2 * dy + 1;
    } else {
      dx--;
      radiusError += 2 * (dy - dx + 1);
    }
  }
}

/** Set the entire cube to one color.

  @param col The color to set all LEDs in the cube to.
*/
void Cube::background(Color col)
{
  for(int x = 0; x < this->size; x++)
    for(int y = 0; y < this->size; y++)
      for(int z = 0; z < this->size; z++)
        this->setVoxel(x, y, z, col);
}

/** Map a value into a color.
  The set of colors fades from blue to green to red and back again.

  @param val Value to map into a color.
  @param minVal Minimum value that val will take.
  @param maxVal Maximum value that val will take.

  @return Color from value.
*/
Color Cube::colorMap(float val, float minVal, float maxVal)
{
  const float range = 1024;
  val = range * (val-minVal) / (maxVal-minVal);

  Color colors[6];

  colors[0].red = 0;
  colors[0].green = 0;
  colors[0].blue = this->maxBrightness;

  colors[1].red = 0;
  colors[1].green = this->maxBrightness;
  colors[1].blue = this->maxBrightness;

  colors[2].red = 0;
  colors[2].green = this->maxBrightness;
  colors[2].blue = 0;

  colors[3].red = this->maxBrightness;
  colors[3].green = this->maxBrightness;
  colors[3].blue = 0;

  colors[4].red = this->maxBrightness;
  colors[4].green = 0;
  colors[4].blue = 0;

  colors[5].red = this->maxBrightness;
  colors[5].green = 0;
  colors[5].blue = this->maxBrightness;

  if(val <= range/6)
    return this->lerpColor(colors[0], colors[1], val, 0, range/6);
  else if(val <= 2 * range / 6)
    return(this->lerpColor(colors[1], colors[2], val, range / 6, 2 * range / 6));
  else if(val <= 3 * range / 6)
    return(this->lerpColor(colors[2], colors[3], val, 2 * range / 6, 3*range / 6));
  else if(val <= 4 * range / 6)
    return(this->lerpColor(colors[3], colors[4], val, 3 * range / 6, 4*range / 6));
  else if(val <= 5 * range / 6)
    return(this->lerpColor(colors[4], colors[5], val, 4 * range / 6, 5*range / 6));
  else
    return(this->lerpColor(colors[5], colors[0], val, 5 * range / 6, range));
}

/** Linear interpolation between colors.

  @param a, b The colors to interpolate between.
  @param val Position on the line between color a and color b.
  When equal to min the output is color a, and when equal to max the output is color b.
  @param minVal Minimum value that val will take.
  @param maxVal Maximum value that val will take.

  @return Color between colors a and b.
*/
Color Cube::lerpColor(Color a, Color b, int val, int minVal, int maxVal)
{
  int red = a.red + (b.red-a.red) * (val-minVal) / (maxVal-minVal);
  int green = a.green + (b.green-a.green) * (val-minVal) / (maxVal-minVal);
  int blue = a.blue + (b.blue-a.blue) * (val-minVal) / (maxVal-minVal);

  return Color(red, green, blue);
}

/** Make changes to the cube visible.
  Causes pixel data to be written to the LED strips.
*/
void Cube::show()
{
  this->strip.show();
  if(this->onlinePressed)
    Particle.process();
}

/** Initialize online/offline switch and the join wifi button */
void Cube::initButtons() {
  //set the input mode for the 'connect to cloud' button
  pinMode(INTERNET_BUTTON, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);

  if(!digitalRead(MODE))
    WiFi.listen();

  //a.k.a. onlinePressed is HIGH when the switch is set to 'online' and LOW when the switch is set to 'offline'
  this->onlinePressed = !digitalRead(INTERNET_BUTTON);

  if(this->onlinePressed)
    Particle.connect();

  void (Cube::*check)(void) = &Cube::onlineOfflineSwitch;
  attachInterrupt(INTERNET_BUTTON, (void (*)())check, CHANGE);

  void (Cube::*wifi)(void) = &Cube::joinWifi;
  attachInterrupt(MODE, (void (*)())wifi, FALLING);
}

/** react to a change of the online/offline switch */
void Cube::onlineOfflineSwitch() {
  // if the 'connect to cloud' button is pressed, try to connect to wifi.  
  // otherwise, run the program

  // onlinePressed is HIGH when the switch is _not_ connected and LOW when the switch is connected
  // a.k.a. onlinePressed is HIGH when the switch is set to 'online' and LOW when the switch is set to 'offline'
  this->onlinePressed = !digitalRead(INTERNET_BUTTON);

  if((!this->onlinePressed) && (this->lastOnline)) {
    //marked as 'online'
    this->lastOnline = this->onlinePressed;
    Particle.connect();
  } else if((this->onlinePressed) && (!this->lastOnline)) {
    // marked as 'offline'
    this->lastOnline = this->onlinePressed;
    Particle.disconnect();
  }

  this->lastOnline = this->onlinePressed;

}

void Cube::joinWifi()
{
    WiFi.listen();
}

/** Listen for the start of UDP streaming. */
void Cube::listen() {
  int32_t bytesrecv = udp.parsePacket();

  // no data, nothing to do
  if(bytesrecv == 0) return;

  if(millis() - this->lastUpdated > 60000) {
    //update the network settings every minute
    this->updateNetworkInfo();
    this->lastUpdated = millis();
  }

  if(bytesrecv == PIXEL_COUNT) {
    char data[512];
    udp.read(data, bytesrecv);

    for(int x = 0; x < this->size; x++) {
      for(int y = 0; y < this->size; y++) {
        for(int z = 0; z < this->size; z++) {
          int index = z*this->size + y*this->size + x;
          Color pixelColor = Color((data[index]&0xE0)>>2, (data[index]&0x1C)<<1, (data[index]&0x03)<<4);   //colors with max brightness set to 64
          this->setVoxel(x, y, z, pixelColor);
        }
      }
    }
  }

  this->show();
}

/** Updates the variables related to the accelerometer
updates accelerometerX, accelerometerY and accelerometerZ, which are directly read from the analog pins, minus 2048 to remove the DC bias

calculates theta and phi, which are the 3D rotation angles
 */
void Cube::updateAccelerometer()
{
  this->accelerometerX=analogRead(X)-2048;
  this->accelerometerY=analogRead(Y)-2048;
  this->accelerometerZ=analogRead(Z)-2048;
  this->theta=atan(this->accelerometerX/sqrt(pow(this->accelerometerY,2)+pow(this->accelerometerZ,2)))*180/3.14;
  this->phi=atan(this->accelerometerY/sqrt(pow(this->accelerometerX,2)+pow(this->accelerometerZ,2)))*180/3.14;
}

/** Update the cube's knowledge of its own network address. */
void Cube::updateNetworkInfo() {
  IPAddress myIp = WiFi.localIP();
  sprintf(this->localIP, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
  byte macAddr[6];
  WiFi.macAddress(macAddr);
  sprintf(this->macAddress, "%02x:%02x:%02x:%02x:%02x:%02x",macAddr[5],macAddr[4],macAddr[3],macAddr[2],macAddr[1],macAddr[0]);
}

/** Function to be called via Spark API for updating the streaming port number.
    Resets the UDP connection with the new port.

    @param _port A decimal number in a String, corresponding to the desired port number.
*/
int Cube::setPort(String _port) {
  this->port = _port.toInt();
  udp.begin(port);
  return port;
}

/** Text functions. */
void Cube::showChar(char a, Point p, Color col) {
    for(int row=0;row<this->size;row++)
        for(int bit=0;bit<8;bit++)
            if(((font[((int)a+this->selectedFont)*8+row]>>(7-bit))&0x01)==1)
                this->setVoxel(p.x+bit, (p.y)+(this->size-1-row), p.z, col);
}

void Cube::showChar(char a, Point origin, Point angle, Color col) {
    showChar(a, origin, Point(0,0,0), angle, col);
}

void Cube::showChar(char a, Point origin, Point pivot, Point angle, Color col) {
    for(int row=0;row<this->size;row++)
        for(int bit=0;bit<8;bit++)
            if(((font[((int)a+this->selectedFont)*8+(7-row)]>>(7-bit))&0x01)==1)
                this->setVoxel(origin.x+((float)bit-pivot.x)*cos(angle.y), 
                origin.y+((float)row-pivot.y)*cos(angle.x), 
                origin.z+((float)row-pivot.y)*sin(angle.x)+((float)bit-pivot.y)*sin(angle.y), col);
}

void Cube::scrollSpinningText(std::string text, Point initialPosition, Color col) {
    for(int i=0;i<text.length();i++)
        this->showChar(text.at(i), Point(this->size*i-initialPosition.x, initialPosition.y, initialPosition.z), Point(0,initialPosition.x,0), col);
}

void Cube::scrollText(std::string text, Point initialPosition, Color col) {
    for(int i=0;i<text.length();i++)
        this->showChar(text.at(i), Point(this->size*i-initialPosition.x, initialPosition.y, initialPosition.z), col);
}

void Cube::marquee(std::string text, float pos, Color col) {
    for(int i=0;i<text.length();i++)
        this->showMarqueeChar(text.at(i), (int)pos - this->size*i, col);
}

void Cube::showMarqueeChar(char a, int pos, Color col) {		 
    for(int row=0;row<this->size;row++)
        for(int bit=0;bit<8;bit++)
            if(((font[((int)a+this->selectedFont)*8+row]>>(7-bit))&0x01)==1) {
                if((pos-bit)<this->size)
                    this->setVoxel(this->size-1, this->size-1-row, pos-bit, col);
                if(((pos-bit)>=this->size)&&((pos-bit)<2*this->size))
                    this->setVoxel((this->size-1)-(pos-bit-this->size), this->size-1-row, this->size-1, col);
                if((pos-bit)>2*this->size)
                    this->setVoxel(0, this->size-1-row, this->size-1-(pos-bit-2*this->size), col);
            }
}