// Test program for Flip Dot display LAWO, 28x24 pixels, with
// 3 chips FP2800A: one for the columns, two for the rows
//
// Connection to Arduino MEGA 1280
// PA --- row
// PC --- columns
// PL --- Panel selection
// 
// Arduino MEGA 1280 | Flip Dot Display
// Port Name | Pin-# | Pin-# | Pin-# | Signal FP2800A
//           |       |  old  |  new  |
// ----------+-------|-----------------------
//   PA0     |   22  |    9  |   17  | Row_A0
//   PA1     |   23  |   10  |   19  | Row_A1
//   PA2     |   24  |   11  |   21  | Row_A2
//   PA3     |   25  |   12  |   23  | Row_B0
//   PA4     |   26  |   13  |   25  | Row_B1
//   PA5     |  n.c. |                
//   PA6     |   28  |   22  |   10  | Row_Enable_on 
//   PA7     |   29  |   23  |   12  | Row_Enable_off
//   PC0     |   37  |    4  |    7  | Col_A0
//   PC1     |   36  |    5  |    9  | Col_A1
//   PC2     |   35  |    6  |   11  | Col_A2
//   PC3     |   34  |    7  |   13  | Col_B0
//   PC4     |   33  |    8  |   15  | Col_B1
//   PC5     |   32  |   21  |    8  | Col_Data
//   PC6     |  n.c.                  
//   PC7     |  n.c.                  
//   PL0     |   49  |   31  |   28  | Col_Enable Panel 1
//   PL1     |   48  |   30  |   26  | Col_Enable Panel 2
//   PL2     |   47  |   29  |   24  | Col_Enable Panel 3
//   PL3     |   46  |   28  |   22  | Col_Enable Panel 4
//   PL4     |   45  |   27  |   20  | Col_Enable Panel 5
//   PL5     |   44  |   26  |   18  | Col_Enable Panel 6
//   PL6     |   43  |   25  |   16  | Col_Enable Panel 7
//   PL7     |   42  |   24  |   14  | Col_Enable Panel 8
//           |       | 15,32 | 29,30 | VS ------------ 10 V DC
//           |       | 16,17 | 31,33 | GND------------ GND
//           |       | 33,34 | 32,34 | GND------------ GND
//    5V     |       |  3,20 |  5,6  | Vcc
//   GND     |       |  1,2  |  1,3  | GND
//   GND     |       | 18,19 |  2,4  | GND

#define PANEL_WIDTH     28 // single Panel width in pixel
#define PANEL_HEIGHT    24 // single Panel height in pixel
#define PANEL_NUMBER     1 // Number of connected Panels
#define PIXELS_WIDTH (PANEL_WIDTH * PANEL_NUMBER)
#define PIXELS_HEIGHT PANEL_HEIGHT

#define LED_PIN         13

//#define TEST2_ALL_ON_OFF
//#define TEST3_RANDOM_PIXELS
//#define TEST4_BOUNCING_BALL
//#define TEST6_SIERPINSKY
//#define TEST7_LETTERS
//#define TEST8_LAPCOUNTER
#define TEST9_SERIAL

char pixels[PANEL_WIDTH*PANEL_NUMBER][PANEL_HEIGHT];
int px, py;
int dx, dy;
int oldx, oldy;
#define PIXEL_PER_CHAR_5x7 6
#define PIXEL_PER_CHAR_3x5 4
uint8_t coord_to_row_col[28] = {1,2,3,4,5,6,7,9,10,11,12,13,14,15,17,18,19,20,21,22,23,25,26,27,28,29,30,31};
uint8_t panel_to_bits[8] = {1,2,4,8,16,32,64,128};
uint8_t font5x7[][5] = {
                    {0x00, 0x00, 0x00, 0x00, 0x00},   // (space)
                    {0x00, 0x00, 0x5F, 0x00, 0x00},   // !
                    {0x00, 0x07, 0x00, 0x07, 0x00},   // "
                    {0x14, 0x7F, 0x14, 0x7F, 0x14},   // #
                    {0x24, 0x2A, 0x7F, 0x2A, 0x12},   // $
                    {0x23, 0x13, 0x08, 0x64, 0x62},   // %
                    {0x36, 0x49, 0x55, 0x22, 0x50},   // &
                    {0x00, 0x05, 0x03, 0x00, 0x00},   // '
                    {0x00, 0x1C, 0x22, 0x41, 0x00},   // (
                    {0x00, 0x41, 0x22, 0x1C, 0x00},   // )
                    {0x08, 0x2A, 0x1C, 0x2A, 0x08},   // *
                    {0x08, 0x08, 0x3E, 0x08, 0x08},   // +
                    {0x00, 0x50, 0x30, 0x00, 0x00},   // ,
                    {0x08, 0x08, 0x08, 0x08, 0x08},   // -
                    {0x00, 0x60, 0x60, 0x00, 0x00},   // .
                    {0x20, 0x10, 0x08, 0x04, 0x02},   // /
                    {0x3E, 0x51, 0x49, 0x45, 0x3E},   // 0
                    {0x00, 0x42, 0x7F, 0x40, 0x00},   // 1
                    {0x42, 0x61, 0x51, 0x49, 0x46},   // 2
                    {0x21, 0x41, 0x45, 0x4B, 0x31},   // 3
                    {0x18, 0x14, 0x12, 0x7F, 0x10},   // 4
                    {0x27, 0x45, 0x45, 0x45, 0x39},   // 5
                    {0x3C, 0x4A, 0x49, 0x49, 0x30},   // 6
                    {0x01, 0x71, 0x09, 0x05, 0x03},   // 7
                    {0x36, 0x49, 0x49, 0x49, 0x36},   // 8
                    {0x06, 0x49, 0x49, 0x29, 0x1E},   // 9
                    {0x00, 0x36, 0x36, 0x00, 0x00},   // :
                    {0x00, 0x56, 0x36, 0x00, 0x00},   // ;
                    {0x00, 0x08, 0x14, 0x22, 0x41},   // <
                    {0x14, 0x14, 0x14, 0x14, 0x14},   // =
                    {0x41, 0x22, 0x14, 0x08, 0x00},   // >
                    {0x02, 0x01, 0x51, 0x09, 0x06},   // ?
                    {0x32, 0x49, 0x79, 0x41, 0x3E},   // @
                    {0x7E, 0x11, 0x11, 0x11, 0x7E},   // A
                    {0x7F, 0x49, 0x49, 0x49, 0x36},   // B
                    {0x3E, 0x41, 0x41, 0x41, 0x22},   // C
                    {0x7F, 0x41, 0x41, 0x22, 0x1C},   // D
                    {0x7F, 0x49, 0x49, 0x49, 0x41},   // E
                    {0x7F, 0x09, 0x09, 0x01, 0x01},   // F
                    {0x3E, 0x41, 0x41, 0x51, 0x32},   // G
                    {0x7F, 0x08, 0x08, 0x08, 0x7F},   // H
                    {0x00, 0x41, 0x7F, 0x41, 0x00},   // I
                    {0x20, 0x40, 0x41, 0x3F, 0x01},   // J
                    {0x7F, 0x08, 0x14, 0x22, 0x41},   // K
                    {0x7F, 0x40, 0x40, 0x40, 0x40},   // L
                    {0x7F, 0x02, 0x04, 0x02, 0x7F},   // M
                    {0x7F, 0x04, 0x08, 0x10, 0x7F},   // N
                    {0x3E, 0x41, 0x41, 0x41, 0x3E},   // O
                    {0x7F, 0x09, 0x09, 0x09, 0x06},   // P
                    {0x3E, 0x41, 0x51, 0x21, 0x5E},   // Q
                    {0x7F, 0x09, 0x19, 0x29, 0x46},   // R
                    {0x46, 0x49, 0x49, 0x49, 0x31},   // S
                    {0x01, 0x01, 0x7F, 0x01, 0x01},   // T
                    {0x3F, 0x40, 0x40, 0x40, 0x3F},   // U
                    {0x1F, 0x20, 0x40, 0x20, 0x1F},   // V
                    {0x7F, 0x20, 0x18, 0x20, 0x7F},   // W
                    {0x63, 0x14, 0x08, 0x14, 0x63},   // X
                    {0x03, 0x04, 0x78, 0x04, 0x03},   // Y
                    {0x61, 0x51, 0x49, 0x45, 0x43},   // Z
                    {0x00, 0x00, 0x7F, 0x41, 0x41},   // [
                    {0x02, 0x04, 0x08, 0x10, 0x20},   // "\"
                    {0x41, 0x41, 0x7F, 0x00, 0x00},   // ]
                    {0x04, 0x02, 0x01, 0x02, 0x04},   // ^
                    {0x40, 0x40, 0x40, 0x40, 0x40},   // _
                    {0x00, 0x01, 0x02, 0x04, 0x00},   // `
                    {0x20, 0x54, 0x54, 0x54, 0x78},   // a
                    {0x7F, 0x48, 0x44, 0x44, 0x38},   // b
                    {0x38, 0x44, 0x44, 0x44, 0x20},   // c
                    {0x38, 0x44, 0x44, 0x48, 0x7F},   // d
                    {0x38, 0x54, 0x54, 0x54, 0x18},   // e
                    {0x08, 0x7E, 0x09, 0x01, 0x02},   // f
                    {0x08, 0x14, 0x54, 0x54, 0x3C},   // g
                    {0x7F, 0x08, 0x04, 0x04, 0x78},   // h
                    {0x00, 0x44, 0x7D, 0x40, 0x00},   // i
                    {0x20, 0x40, 0x44, 0x3D, 0x00},   // j
                    {0x00, 0x7F, 0x10, 0x28, 0x44},   // k
                    {0x00, 0x41, 0x7F, 0x40, 0x00},   // l
                    {0x7C, 0x04, 0x18, 0x04, 0x78},   // m
                    {0x7C, 0x08, 0x04, 0x04, 0x78},   // n
                    {0x38, 0x44, 0x44, 0x44, 0x38},   // o
                    {0x7C, 0x14, 0x14, 0x14, 0x08},   // p
                    {0x08, 0x14, 0x14, 0x18, 0x7C},   // q
                    {0x7C, 0x08, 0x04, 0x04, 0x08},   // r
                    {0x48, 0x54, 0x54, 0x54, 0x20},   // s
                    {0x04, 0x3F, 0x44, 0x40, 0x20},   // t
                    {0x3C, 0x40, 0x40, 0x20, 0x7C},   // u
                    {0x1C, 0x20, 0x40, 0x20, 0x1C},   // v
                    {0x3C, 0x40, 0x30, 0x40, 0x3C},   // w
                    {0x44, 0x28, 0x10, 0x28, 0x44},   // x
                    {0x0C, 0x50, 0x50, 0x50, 0x3C},   // y
                    {0x44, 0x64, 0x54, 0x4C, 0x44},   // z
                    {0x00, 0x08, 0x36, 0x41, 0x00},   // {
                    {0x00, 0x00, 0x7F, 0x00, 0x00},   // |
                    {0x00, 0x41, 0x36, 0x08, 0x00},   // }
                    {0x08, 0x08, 0x2A, 0x1C, 0x08},   // ->
                    {0x08, 0x1C, 0x2A, 0x08, 0x08}    // <-
};

uint8_t font3x5[][3] = {
                     {0x1F, 0x11, 0x1F},  // 0
                     {0x09, 0x7F, 0x01},  // 1
                     {0x17, 0x15, 0x1D},  // 2
                     {0x15, 0x15, 0x1F},  // 3
                     {0x1C, 0x04, 0x1F},  // 4
                     {0x1D, 0x15, 0x12},  // 5
                     {0x1F, 0x15, 0x17},  // 6
                     {0x13, 0x14, 0x1C},  // 7
                     {0x1F, 0x15, 0x1F},  // 8
                     {0x1D, 0x15, 0x1F},  // 9
                     {0x1B, 0x1B, 0x00},  // :
                     {0x6D, 0x6E, 0x00},  // ;
                     {0x14, 0x22, 0x41},  // <
                     {0x14, 0x14, 0x14},  // =
                     {0x41, 0x22, 0x14},  // >
                     {0x40, 0x45, 0x48},  // ?
                     {0x41, 0x79, 0x49},  // @
                     {0x1F, 0x14, 0x1F},  // A
                     {0x1F, 0x15, 0x0A},  // B
                     {0x0E, 0x11, 0x11},  // C
                     {0x1F, 0x11, 0x0E},  // D
                     {0x1F, 0x15, 0x15},  // E
                     {0x1F, 0x14, 0x14},  // F
                     {0x0E, 0x11, 0x17},  // G
                     {0x1F, 0x04, 0x1F},  // H
                     {0x00, 0x1F, 0x00},  // I
                     {0x12, 0x11, 0x1E},  // J
                     {0x1F, 0x04, 0x1B},  // K
                     {0x1F, 0x01, 0x01},  // L
                     {0x1F, 0x0C, 0x1F},  // M
                     {0x1F, 0x0E, 0x1F},  // N
                     {0x1F, 0x11, 0x1F},  // O
                     {0x1F, 0x14, 0x1C},  // P
                     {0x1F, 0x13, 0x1E},  // Q
                     {0x1F, 0x16, 0x09},  // R
                     {0x1D, 0x15, 0x17},  // S
                     {0x10, 0x1F, 0x10},  // T
                     {0x1F, 0x01, 0x1F},  // U
                     {0x1E, 0x01, 0x1E},  // V
                     {0x1F, 0x02, 0x1F},  // W
                     {0x1B, 0x04, 0x1B},  // X
                     {0x18, 0x07, 0x18},  // Y
                     {0x13, 0x15, 0x19},  // Z
};

int cmd_buffer_max = 100;
int cmd_buffer_index = 0;
char cmd_buffer[100];
int cursor_x = 0;  // x-coordinate to where the next command prints
int cursor_y = 0;  // y-coordinate to where the next command prints
int fontID = 5;    // Font to be used.  3 = 3x5, 5 = 5x7

//--------------------------------------------------------------
// Set or clear a pixel at coordinates (col, row)
void setPixel(uint8_t col, uint8_t row, bool on)
{
  if ((col < 0) || (col > ((PANEL_WIDTH * PANEL_NUMBER) - 1)) || (row < 0) || (row > (PANEL_HEIGHT - 1))) return;
  if (pixels[col][row] == on) return;  //saving time on switching pixels already in the correct state
  pixels[col][row] = on;

  // Translate Column to Panel
  uint8_t panel = col/PANEL_WIDTH; 
  col = col - (panel * PANEL_WIDTH);
  // Translate pixel coordinates to row and column address
  // This is necessary because the FP2800A chip skips the
  // addresses 0, 8, 16, 24
  row = coord_to_row_col[row];
  col = coord_to_row_col[col];
  // Translate the panelnumber into correct bits
  panel = panel_to_bits[panel];
  
  // Set row address
  PORTA = (row & 0x1F);
  // Set column address
  PORTC = (col & 0x1F);
  // Set Data signal
  if (on)
    PORTC |= 0x20;
  else
    PORTC &= 0xDF;
    
  // Wait t_SE  
  delayMicroseconds(50);
  
  // Set Enable
  PORTL = (panel & 0xFF);  // Col_Enable panel x
  if (on)
  {
    PORTA |= 0x40;  // Row_Enable_on
//    PORTC |= 0x40;  // Col_Enable
  }
  else
  {
    PORTA |= 0x80;  // Row_Enable_off
//    PORTC |= 0x40;  // Col_Enable
  }
  
  // Give the dots time to flip
  delayMicroseconds(600);
    
  // Reset Data and Enable lines
  PORTA &= 0x1F;
  PORTC &= 0x1F;
  PORTL = 0x00;
  
  // Wait t_off
  delayMicroseconds(150);
}  


//--------------------------------------------------------
void clearDisplay()
{
  fillRect(0, 0, ((PANEL_WIDTH * PANEL_NUMBER) - 1) , (PANEL_HEIGHT - 1), false);
}


//--------------------------------------------------------
void fillDisplay()
{
  fillRect(0, 0, ((PANEL_WIDTH * PANEL_NUMBER) - 1) , (PANEL_HEIGHT - 1), true);
}


//--------------------------------------------------------
// Print a letter at (x, y) = top-left corner of the letter
// A letter takes up 5x7 pixels. 
// Return the width of the letter in pixels.
int printLetter5x7(char character, int x, int y)
{
  uint8_t* letter = font5x7[character - '!' + 1];
 
  // Go through the 5 columns of the letter
  for (int col = 0;  col < 5;  col++)
  {
    uint8_t bits = letter[col];
    // Go through the pixels of the letter
    for (int row = 6;  row >= 0;  row--)
    {
      uint8_t pixel = bits & 0x01;
      bits >>= 1;
      setPixel(x + col, 7 + y - row, pixel);
    }
  }
  return PIXEL_PER_CHAR_5x7;
}


//--------------------------------------------------------
// Print a letter at (x, y) = top-left corner of the letter
// A letter takes up 3x5 pixels.
int printLetter3x5(char character, int x, int y)
{
  // convert lower case to upper case
//  if ((character >= 'a')  && (character <= 'z'))
//  {
//    character -= 'a' + 'A';
//  }
  character = toupper(character);
  uint8_t* letter = font3x5[character - '0'];
    
  // Go through the 3 columns of the letter
  for (int col = 0;  col < 3;  col++)
  {
    uint8_t bits = letter[col];
    // Go through the pixels of the letter
    for (int row = 4;  row >= 0;  row--)
    {
      uint8_t pixel = bits & 0x01;
      bits >>= 1;
      setPixel(x + col, y + row, pixel);
    }
  }

  return PIXEL_PER_CHAR_3x5;
}


//--------------------------------------------------------
void fillRect(int x1, int y1, int x2, int y2, bool on)
{
  for (int y = y1;  y <= y2;  y++) 
    for (int x = x1;  x <= x2;  x++)
      setPixel(x, y, on);
}


//--------------------------------------------------------
void drawRect(int x1, int y1, int x2, int y2, bool on)
{
  for (int x = x1;  x <= x2;  x++) setPixel(x, y1, on);
  for (int x = x1;  x <= x2;  x++) setPixel(x, y2, on);
  for (int y = y1;  y <= y2;  y++) setPixel(x1, y, on);
  for (int y = y1;  y <= y2;  y++) setPixel(x2, y, on);
}  


//--------------------------------------------------------
void drawRoundedRect(int x1, int y1, int x2, int y2, bool on)
{
  for (int x = x1 + 1;  x <= x2 - 1;  x++) setPixel(x, y1, on);
  for (int x = x1 + 1;  x <= x2 - 1;  x++) setPixel(x, y2, on);
  for (int y = y1 + 1;  y <= y2 - 1;  y++) setPixel(x1, y, on);
  for (int y = y1 + 1;  y <= y2 - 1;  y++) setPixel(x2, y, on);
}  


//--------------------------------------------------------
void drawFilledCircle(int x, int y, bool on)
{
  setPixel(x + 1, y, on);
  setPixel(x + 2, y, on);
  setPixel(x + 3, y, on);
  for (int y1 = y + 1;  y1 < y + 4;  y1++)
    for (int x1 = x;  x1 < x + 5;  x1++)
      setPixel(x1, y1, on);
  setPixel(x + 1, y + 4, on);
  setPixel(x + 2, y + 4, on);
  setPixel(x + 3, y + 4, on);
}  


//----------------------------------------------------------
void drawAmpel(int x, int y, bool on)
{
  drawRoundedRect( x, y, x + 4, y + 4, on);
  drawFilledCircle(x, y +  5, on);
  drawFilledCircle(x, y + 10, on);
}


//--------------------------------------------------------
// Pong 
// Helper function
void paddle(int x, int y)
{
  x -= 2;
  if (x < 0)  x = 0;
  if (x > 19) x = 19;
  for (int i = 0;  i < 5;  i++) 
    setPixel(x + i, y, true);
    
  if (x > 0)  setPixel(x - 1, y, false);
  if (x < (PANEL_WIDTH-1)) setPixel(x + 5, y, false);
}

void pong()
{
  setPixel(oldx, oldy, false);
  
  oldx = px;
  oldy = py;
  
  px += dx;
  py += dy;
  if (px == (PANEL_WIDTH - 1)) dx = -dx;
  if (px == 0)  dx = -dx;
  if (py == (PANEL_HEIGHT - 1)) dy = -dy;
  if (py == 0)  dy = -dy;
  
  setPixel(px, py, true);
  paddle(px, 0);
  paddle(px, (PANEL_WIDTH - 1));
  delay(10);
}


//------------------------------------------------
// Sierpinsky triangle
void sier()
{
  for (int x = 0;  x < (PANEL_WIDTH * PANEL_NUMBER);  x++) 
    for (int y = 0;  y < PANEL_HEIGHT;  y++) 
      setPixel(x, y, !(x & y));
}


//--------------------------------------------------------
//--------------------------------------------------------
void setup()
{
  // Port A, C & L = output
  DDRA = 0xFF;
  DDRC = 0xFF;
  DDRL = 0xFF;
  // Set all pins to LOW
  PORTA = 0x00;
  PORTC = 0x00;
  PORTL = 0x00;
  
  // Required for the various effects
  dx = 1;
  dy = 1;
  px = 0;
  py = 2;
  oldx = 0;
  oldy = 0;

  // initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);     

  // Serial 1 is the command port
  Serial1.begin(115200);
  
  // Fill the display
  Serial1.write("Fill Display\r\n");
  fillDisplay();
  // Clear the display
  Serial1.write("Clear Display\r\n");
  clearDisplay();
  
  Serial1.write("Ready.\r\n");
  
#ifdef TEST9_SERIAL
  printHelp();
#endif

}


//--------------------------------------------------------
//-------------------------------------------------
// Main loop
void loop()
{
  // put your main code here, to run repeatedly:
#ifdef TEST1_PIXEL_ON_OFF
  setPixel(1, 1, true);
  setPixel(2, 1, true);
  setPixel(3, 1, true);
  
  delay(2000);

  setPixel(1, 1, false);
  setPixel(2, 1, false);
  setPixel(3, 1, false);
  
  delay(2000);
#endif

#ifdef TEST2_ALL_ON_OFF
  // Fill the display
  fillDisplay();
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(4000);
  // Clear the display
  clearDisplay();
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(4000);
#endif

#ifdef TEST3_RANDOM_PIXELS
  setPixel(random(1, (PANEL_WIDTH * PANEL_NUMBER)), random(1, PANEL_HEIGHT), random(0, 2));
#endif

#ifdef TEST4_BOUNCING_BALL
  px += dx;
  py += dy;
  if (px == (PANEL_WIDTH * PANEL_NUMBER)-1) dx = -dx;
  if (px == 0)  dx = -dx;
  if (py == PANEL_HEIGHT-1) dy = -dy;
  if (py == 0)  dy = -dy;
  if (pixels[px][py] == 0)
  {
      pixels[px][py] = 1;
      setPixel(px, py, 1);
  }
  else
  {
      pixels[px][py] = 0;
      setPixel(px, py, 0);
  }   
 delay(5);
#endif

#ifdef TEST5_PONG 
 pong();
#endif

#ifdef TEST6_SIERPINSKY
 sier();
 while(1); 
#endif

#ifdef TEST7_LETTERS
  printLetter5x7('0',  0,  0);
  printLetter5x7('1',  6,  0);
  printLetter5x7('2', 12,  0);
  printLetter5x7('3', 18,  0);
  
  printLetter5x7('4',  0,  8);
  printLetter5x7('5',  6,  8);
  printLetter5x7('6', 12,  8);
  printLetter5x7('7', 18,  8);
  
  printLetter5x7('8',  0, 16);
  printLetter5x7('9',  6, 16);
  printLetter5x7(':', 12, 16);
  printLetter5x7(';', 18, 16);
  
  delay(5000);
  printLetter5x7('<',  0,  0);
  printLetter5x7('=',  6,  0);
  printLetter5x7('>', 12,  0);
  printLetter5x7('?', 18,  0);
  
  fillRect(0, 8, (PANEL_WIDTH - 1), (PANEL_HEIGHT - 1), false);
  printLetter5x7('@',  0,  8);
  
  delay(5000);
  
  printLetter5x7('A',  0,  0);
  printLetter5x7('B',  6,  0);
  printLetter5x7('C', 12,  0);
  printLetter5x7('D', 18,  0);
  
  printLetter5x7('E',  0,  8);
  printLetter5x7('F',  6,  8);
  printLetter5x7('G', 12,  8);
  printLetter5x7('H', 18,  8);
  
  printLetter5x7('I',  0, 16);
  printLetter5x7('J',  6, 16);
  printLetter5x7('K', 12, 16);
  printLetter5x7('L', 18, 16);
  
  delay(5000);
  
  printLetter5x7('M',  0,  0);
  printLetter5x7('N',  6,  0);
  printLetter5x7('O', 12,  0);
  printLetter5x7('P', 18,  0);
  
  printLetter5x7('Q',  0,  8);
  printLetter5x7('R',  6,  8);
  printLetter5x7('S', 12,  8);
  printLetter5x7('T', 18,  8);
  
  printLetter5x7('U',  0, 16);
  printLetter5x7('V',  6, 16);
  printLetter5x7('W', 12, 16);
  printLetter5x7('X', 18, 16);
  
  delay(5000);
  printLetter5x7('a',  0,  0);
  printLetter5x7('b',  6,  0);
  printLetter5x7('c', 12,  0);
  printLetter5x7('d', 18,  0);
  
  printLetter5x7('e',  0,  8);
  printLetter5x7('f',  6,  8);
  printLetter5x7('g', 12,  8);
  printLetter5x7('h', 18,  8);
  
  printLetter5x7('i',  0, 16);
  printLetter5x7('j',  6, 16);
  printLetter5x7('k', 12, 16);
  printLetter5x7('l', 18, 16);
  
  delay(5000);
  
  printLetter5x7('m',  0,  0);
  printLetter5x7('n',  6,  0);
  printLetter5x7('o', 12,  0);
  printLetter5x7('p', 18,  0);
  
  printLetter5x7('q',  0,  8);
  printLetter5x7('r',  6,  8);
  printLetter5x7('s', 12,  8);
  printLetter5x7('t', 18,  8);
  
  printLetter5x7('u',  0, 16);
  printLetter5x7('v',  6, 16);
  printLetter5x7('w', 12, 16);
  printLetter5x7('x', 18, 16);
  
  delay(5000);
#endif

#ifdef TEST8_LAPCOUNTER
  delay(5000);

  drawAmpel(0, 5, true);
  delay(1000);
  drawAmpel(6, 5, true);
  delay(1000);
  drawAmpel(12, 5, true);
  delay(1000);
  drawAmpel(17, 5, true);
  delay(1000);
  drawAmpel(23, 5, true);
  delay(2300);
  
  fillRect(0, 5, 27, 20, false);
  printLetter5x7('G',  9, 10);
  printLetter5x7('O', 15, 10);
  printLetter5x7('[', 22, 10);  // is really '!'  
  printLetter5x7('[', 24, 10);  // is really '!'  

  delay(2000);
  clearDisplay();
  
  printLetter5x7('L',  0, 0);
  printLetter5x7('E',  6, 0);
  printLetter5x7('Z', 12, 0);
  
  drawRoundedRect(19, 0, 19+8, 0+8, true);
  printLetter3x5('2', 22, 2);
  
  printLetter5x7('2',   0,  9);
  printLetter5x7('3',   6,  9);
  printLetter3x5('L',  13, 11);
  printLetter3x5('A',  17, 11);
  printLetter3x5('P',  21, 11);
  printLetter3x5('S',  25, 11);
  
  printLetter5x7('1',  1, 17);
  printLetter5x7('7',  6, 17);
  printLetter3x5(':', 12, 18);
  printLetter5x7('6', 15, 17);
  printLetter5x7('2', 21, 17);

  delay(5000);
  
  printLetter5x7('A',  0, 0);
  printLetter5x7('L',  6, 0);
  printLetter5x7('X', 12, 0);
  
  drawRoundedRect(19, 0, 19+8, 0+8, true);
  printLetter3x5('1', 22, 2);
  
  printLetter5x7('2',   0,  9);
  printLetter5x7('5',   6,  9);
  printLetter3x5('L',  13, 11);
  printLetter3x5('A',  17, 11);
  printLetter3x5('P',  21, 11);
  printLetter3x5('S',  25, 11);
  
  printLetter5x7('1',  1, 17);
  printLetter5x7('5',  6, 17);
  printLetter3x5(':', 12, 18);
  printLetter5x7('8', 15, 17);
  printLetter5x7('3', 21, 17);
  
  while(1);
  delay(5000);
#endif


#ifdef TEST9_SERIAL
  while (Serial1.available())
  {
    cmd_buffer[cmd_buffer_index] = Serial1.read();
    Serial1.write(cmd_buffer[cmd_buffer_index]);
    // Empty line?
    if (((cmd_buffer[cmd_buffer_index] == '\n') || (cmd_buffer[cmd_buffer_index] == '\r'))
       && (cmd_buffer_index == 0))
    {
      // nothing, ignore empty line
    }
    // Command complete?
    else if ((cmd_buffer[cmd_buffer_index] == '\n') || (cmd_buffer[cmd_buffer_index] == '\r'))
    {
      handle_command();

      cmd_buffer_index = 0;
    }
    // Command not complete
    else
    {
      if (cmd_buffer_index < cmd_buffer_max - 1)
      {
        cmd_buffer_index++;
      }
    }
  } // while Serial1.available()
#endif

}


//-------------------------------------------------------------------------------------------
// Process a command that was received in cmd_buffer and consists of
// comd_buffer_index characters
void handle_command()
{
  //-----------------------------------------
  if (cmd_buffer[0] == '?')
  {
    printHelp();
  }

  //-----------------------------------------
  // 3<string>  = Text at the current cursor position, use 3x5 font
  // g0,0
  // 3ALEX
  else if (cmd_buffer[0] == '3')
  {
    Serial1.write("\nPrint Text 3x5\r\n");
    for (int i = 1;
         i < min(cmd_buffer_index, 1 + PIXELS_WIDTH / PIXEL_PER_CHAR_3x5);
         i++)
    {
      cursor_x += printLetter3x5(cmd_buffer[i], cursor_x, cursor_y);
    }
  }

  //-----------------------------------------
  // 5<string>  = Text at the current cursor position, use 5x7 font
  // g0,8
  // 5Alex
  else if (cmd_buffer[0] == '5')
  {
    Serial1.write("\nPrint Text\r\n");
    for (int i = 1;
         i < min(cmd_buffer_index, 1 + PIXELS_WIDTH / PIXEL_PER_CHAR_5x7);
         i++)
    {
      cursor_x += printLetter5x7(cmd_buffer[i], cursor_x, cursor_y);
    }
  }

  //-----------------------------------------
  // C  = Clear Display
  // g0,0
  // 5TEST
  // C
  else if (cmd_buffer[0] == 'C')
  {
    Serial1.write("Clear Display\r\n");
    clearDisplay();
    cursor_x = 0;
    cursor_y = 0;
  }

  //-----------------------------------------
  // d  = Print Dimensions  d<int>,<int>
  // d
  else if (cmd_buffer[0] == 'd')
  {
    Serial1.print("d");
    Serial1.print(PIXELS_WIDTH);
    Serial1.print(",");
    Serial1.println(PIXELS_HEIGHT);
  }

  //-----------------------------------------
  // f<x1>,<y1>,<x2>,<y2>  = Fill Rectangle Area
  // C
  // f4,4,15,15
  else if (cmd_buffer[0] == 'f')
  {
    int idx = 1;
    int x1 = readNumber(&idx);
    x1 = constrain(x1, 0, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y1 = readNumber(&idx);
    y1 = constrain(y1, 0, PIXELS_HEIGHT - 1);
    idx++;  // to skip the ','
    int x2 = readNumber(&idx);
    x2 = constrain(x2, x1, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y2 = readNumber(&idx);
    y2 = constrain(y2, y1, PIXELS_HEIGHT - 1);
    fillRect(x1, y1, x2, y2, true);
  }

  //-----------------------------------------
  // F<x1>,<y1>,<x2>,<y2>  = Erase Rectangle Area
  // C
  // f4,4,15,15
  // F4,4,15,15
  else if (cmd_buffer[0] == 'F')
  {
    int idx = 1;
    int x1 = readNumber(&idx);
    x1 = constrain(x1, 0, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y1 = readNumber(&idx);
    y1 = constrain(y1, 0, PIXELS_HEIGHT - 1);
    idx++;  // to skip the ','
    int x2 = readNumber(&idx);
    x2 = constrain(x2, x1, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y2 = readNumber(&idx);
    y2 = constrain(y2, y1, PIXELS_HEIGHT - 1);
    fillRect(x1, y1, x2, y2, false);
  }

  //-----------------------------------------
  // g<x>,<y>  = GotoXY
  // g8,10
  // 58,10
  // Invalid numbers result in 0
  // Too large or too small numbers are limited to the display size.
  else if (cmd_buffer[0] == 'g')
  {
    int idx = 1;
    int xpos = readNumber(&idx);
    idx++;  // to skip the ','
    int ypos = readNumber(&idx);
    cursor_x = constrain(xpos, 0, PIXELS_WIDTH - 1);
    cursor_y = constrain(ypos, 0, PIXELS_HEIGHT - 1);
    Serial1.print("Goto x = ");
    Serial1.print(cursor_x);
    Serial1.print(", y = ");
    Serial1.println(cursor_y);
  }

  //------------------------------------------
  // p<x>,<y>  = Set Pixel at x1,y1
  // C
  // p0,0
  // p12,12
  // p13,13
  // p14,14
  else if (cmd_buffer[0] == 'p')
  {
    int idx = 1;
    int xpos = readNumber(&idx);
    idx++;  // to skip the ','
    int ypos = readNumber(&idx);
    cursor_x = constrain(xpos, 0, PIXELS_WIDTH - 1);
    cursor_y = constrain(ypos, 0, PIXELS_HEIGHT - 1);
    setPixel(xpos, ypos, true);
  }

  //------------------------------------------
  // P<x>,<y>  = Clear Pixel at x1,y1
  // C
  // f0,0,20,20
  // P13,13
  // P0,0
  else if (cmd_buffer[0] == 'P')
  {
    int idx = 1;
    int xpos = readNumber(&idx);
    idx++;  // to skip the ','
    int ypos = readNumber(&idx);
    cursor_x = constrain(xpos, 0, PIXELS_WIDTH - 1);
    cursor_y = constrain(ypos, 0, PIXELS_HEIGHT - 1);
    setPixel(xpos, ypos, false);
  }

  //-----------------------------------------
  // r<x1>,<y1>,<x2>,<y2>  = Draw Rectangle (not filled)
  // C
  // r4,4,15,15
  else if (cmd_buffer[0] == 'r')
  {
    int idx = 1;
    int x1 = readNumber(&idx);
    x1 = constrain(x1, 0, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y1 = readNumber(&idx);
    y1 = constrain(y1, 0, PIXELS_HEIGHT - 1);
    idx++;  // to skip the ','
    int x2 = readNumber(&idx);
    x2 = constrain(x2, x1, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y2 = readNumber(&idx);
    y2 = constrain(y2, y1, PIXELS_HEIGHT - 1);
    drawRect(x1, y1, x2, y2, true);
  }

  //-----------------------------------------
  // R<x1>,<y1>,<x2>,<y2>  = Erase Rectangle (not filled)
  // C
  // f4,4,15,15
  // R5,5,14,14
  else if (cmd_buffer[0] == 'R')
  {
    int idx = 1;
    int x1 = readNumber(&idx);
    x1 = constrain(x1, 0, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y1 = readNumber(&idx);
    y1 = constrain(y1, 0, PIXELS_HEIGHT - 1);
    idx++;  // to skip the ','
    int x2 = readNumber(&idx);
    x2 = constrain(x2, x1, PIXELS_WIDTH - 1);
    idx++;  // to skip the ','
    int y2 = readNumber(&idx);
    y2 = constrain(y2, y1, PIXELS_HEIGHT - 1);
    drawRect(x1, y1, x2, y2, false);
  }

  //------------------------------------------
  // s<fontID>  = Set Font ID (3 = 3x5, 5 = 5x7)
  // An illegal font-ID does not change the font-ID
  // C
  // s3
  // t3X5
  // g0,8
  // s5
  // t5x7
  else if (cmd_buffer[0] == 's')
  {
    int idx = 1;
    int id = readNumber(&idx);
    if ((id == 3) || (id == 5))
    {
      fontID = id;
    }
    // otherwise no change
  }

  //-----------------------------------------
  // t<string>  = Text at the current cursor position, use current font
  // C
  // s3
  // tALEX
  // s5
  // g5,10
  // talx
  else if (cmd_buffer[0] == 't')
  {
    for (int i = 1;
         ((i < cmd_buffer_index) && (cursor_x < PIXELS_WIDTH - PIXEL_PER_CHAR_3x5));
         i++)
    {
      if (fontID == 3)
      {
        cursor_x += printLetter3x5(cmd_buffer[i], cursor_x, cursor_y);

      }
      else if (fontID == 5)
      {
        cursor_x += printLetter5x7(cmd_buffer[i], cursor_x, cursor_y);
      }
    }
  }
  Serial1.println();
}


//--------------------------------------------------------------------------------------------
// Convert an ASCII-number into an int
int readNumber(int *index)
{
  int number = 0;
  while((*index < cmd_buffer_index)
     && (cmd_buffer[*index] >= '0') && (cmd_buffer[*index] <= '9'))
  {
    number = 10 * number + cmd_buffer[*index] - '0';
    (*index)++;
  }
  Serial1.print("ReadNumber returns ");
  Serial1.println(number); 
  return number;
}


//-------------------------------------------------------------------------------------------
// Print command help
void printHelp()
 {
    Serial1.println("Commands:");
    Serial1.println("-----------------------------------------------------------------");
    Serial1.println("3<string>             = Text at the current cursor position, use 3x5 font");
    Serial1.println("5<string>             = Text at the current cursor position, use 5x7 font");
    Serial1.println("C                     = Clear Display");
    Serial1.println("d                     = Print Dimensions  d<int>,<int>");
    Serial1.println("f<x1>,<y1>,<x2>,<y2>  = Fill Rectangle Area");
    Serial1.println("F<x1>,<y1>,<x2>,<y2>  = Erase Rectangle Area");
    Serial1.println("g<x>,<y>              = GotoXY");
    Serial1.println("p<x>,<y>              = Set Pixel at x1,y1");
    Serial1.println("P<x>,<y>              = Clear Pixel at x1,y1");
    Serial1.println("r<x1>,<y1>,<x2>,<y2>  = Draw Rectangle (not filled)");
    Serial1.println("R<x1>,<y1>,<x2>,<y2>  = Erase Rectangle (not filled)");
    Serial1.println("s<fontID>             = Set Font ID (3 = 3x5, 5 = 5x7)");
    Serial1.println("t<string>             = Text at the current cursor position, use current font");
    Serial1.println();
}

