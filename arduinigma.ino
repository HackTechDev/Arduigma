#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <SeeedTouchScreen.h> 

// Logo

const __FlashStringHelper * LOGOBSF;
const __FlashStringHelper * LOGOSF;

// Touchscreen initialization

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3 

#elif defined(__AVR_ATmega32U4__) // leonardo
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 18   // can be a digital pin, this is A0
#define XP 21   // can be a digital pin, this is A3 

#else //168, 328, something else
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3 

#endif

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM);

char keyboardRow1st[] = "QWERTZUIO"; // 9
char keyboardRow2nd[] = "ASDFGHJK"; // 8
char keyboardRow3rd[] = "PYXCVBNML"; // 9

int x = 0;    // variable
int i = 0;
int alphabetLetter;
int letterCounterByLine = 0;

int touchDeltaX = 0;
int touchDeltaY = 0;

int keyboardLeftBorderRow1st;
int keyboardLeftBorderRow2nd;
int keyboardPosY;

int keyboardCirclePosX;
int keyboardCirclePosY;
int keyboardCircleRadius = 11; 
int keyboardLetterSquare = keyboardCircleRadius * 2;



char message[10];

char messageChar;

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:    
   
  TFT_BL_ON;      // turn on the background light
  Tft.TFTinit();  // init TFT library
   
  Tft.drawString("Arduigma", 0, 0, 2, WHITE); 
   
  // Logo
  
  
  LOGOBSF = F("\x00\x00\x03\xFF\xFC\x00\x00\x00\x00\x00\xFF\xFF\xFF\xF0\x00\x00\x00\x07\xFF\xFF\xFF\xFE\x00\x00\x00\x3F\xFF\xFF\xFF\xFF\xC0\x00\x00\xFF\xFF\xFF"
              "\xFF\xFF\xF0\x00\x03\xFF\xFF\xFF\xFF\xFF\xFC\x00\x07\xFF\xFF\xFF\xFF\xFF\xFE\x00\x0F\xFF\xFF\xFF\xFF\xFF\xFF\x00\x1F\xFF\xFF\xFF\xFF\xFF\xFF\x80"
              "\x3F\xFF\xFF\xFF\xFF\xFF\xFF\xC0\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xE0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xF0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xF0\xFF\xFF\xFF\xFF"
              "\xFF\xFF\xFF\xF0\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xE0\x3F\xFF\xFF\xFF\xFF\xFF\xFF\xC0\x1F\xFF\xFF\xFF\xFF\xFF\xFF\x80\x0F\xFF\xFF\xFF\xFF\xFF\xFF\x00"
              "\x07\xFF\xFF\xFF\xFF\xFF\xFE\x00\x03\xFF\xFF\xFF\xFF\xFF\xFC\x00\x00\xFF\xFF\xFF\xFF\xFF\xF0\x00\x00\x3F\xFF\xFF\xFF\xFF\xC0\x00\x00\x07\xFF\xFF"
              "\xFF\xFE\x00\x00\x00\x00\xFF\xFF\xFF\xF0\x00\x00\x00\x00\x03\xFF\xFC\x00\x00\x00");

  LOGOSF = F("\x00\x00\x03\xFF\xFC\x00\x00\x00\x00\x00\xFF\xFF\xFF\xF0\x00\x00\x00\x07\xFC\x00\x03\xFE\x00\x00\x00\x3F\x00\x00\x00\x0F\xC0\x00\x00\xF8\x01\x8F"
             "\x1F\x01\xF0\x00\x03\xC0\xF9\x9F\x9F\xF0\x3C\x00\x07\x03\xF9\x99\x99\xF8\x0E\x00\x0C\x33\x19\x98\x19\x99\x83\x00\x19\xF3\x19\x98\x19\x99\xE1\x80"
             "\x39\x83\x19\x98\x19\x99\xF9\xC0\x79\x83\x19\x98\x19\x99\x99\xE0\xDD\xE3\x19\x9B\x99\x99\x9B\xB0\x8D\xE3\x19\x9B\x99\x99\xFB\x10\xDD\x83\x19\x99"
             "\x99\x99\xFB\xB0\x79\x83\x19\x99\x99\x99\x99\xE0\x39\xC3\x19\x99\x99\x99\x99\xC0\x19\xF3\x19\x99\x99\x99\x91\x80\x0C\x33\x19\x99\x99\x99\x83\x00"
             "\x07\x03\x19\x99\x99\x99\x0E\x00\x03\xC0\x19\x9F\x99\x90\x3C\x00\x00\xF8\x19\x8F\x19\x01\xF0\x00\x00\x3F\x00\x00\x00\x0F\xC0\x00\x00\x07\xFC\x00"
             "\x03\xFE\x00\x00\x00\x00\xFF\xFF\xFF\xF0\x00\x00\x00\x00\x03\xFF\xFC\x00\x00\x00");

  DrawLogo();
   
  // Keyboard 1st row
  
  keyboardPosY = 230;
  keyboardLeftBorderRow1st = 15;
  keyboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow1st) - 1; letterCounterByLine++) {
    Tft.drawChar(keyboardRow1st[letterCounterByLine], keyboardLeftBorderRow1st + (letterCounterByLine * 25), keyboardPosY, 2, WHITE);
    keyboardCirclePosY = keyboardPosY + 7;
    Tft.drawCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, WHITE);
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  }
   
  // Keyboard 2nd row
  
  keyboardPosY = keyboardPosY + 25;
  keyboardLeftBorderRow2nd = 25;
  keyboardCirclePosX = 32;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow2nd) - 1; letterCounterByLine++) {
    Tft.drawChar(keyboardRow2nd[letterCounterByLine], keyboardLeftBorderRow2nd + (letterCounterByLine * 25), keyboardPosY, 2, WHITE);
    keyboardCirclePosY = keyboardPosY + 7;
    Tft.drawCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, WHITE);
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  } 
   
  
  // Keyboard 3rd row
  
  keyboardPosY = keyboardPosY + 25;
  keyboardLeftBorderRow1st = 15;
  keyboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow3rd) - 1; letterCounterByLine++) {
    Tft.drawChar(keyboardRow3rd[letterCounterByLine], keyboardLeftBorderRow1st + (letterCounterByLine * 25), keyboardPosY, 2, WHITE);
    keyboardCirclePosY = keyboardPosY + 7;
    Tft.drawCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, WHITE);
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  }


}

void loop() {
  
  messageChar = detectButtonLetter();
  
  if(messageChar != ' ' ) {
    Serial.println(messageChar);
  }
  
  delay(100);  
}

char detectButtonLetter() {

  Point p = ts.getPoint();

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);


  
  if (p.z > __PRESURE) {

   Serial.print("presureX = "); Serial.print(p.x); Serial.print(" presureY = "); Serial.println(p.y);  
   

    // Test 1st row
    keyboardPosY = 230;
    keyboardLeftBorderRow1st = 15;
    keyboardCirclePosX = 22;    
    keyboardCirclePosY = keyboardPosY + 7;    
    for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow1st) - 1; letterCounterByLine++) {     
      if (p.x > (keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) && 
          p.y > (keyboardCirclePosY - keyboardCircleRadius) + touchDeltaY &&
          p.x < ((keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) + keyboardLetterSquare)  && 
          p.y < ((keyboardCirclePosY - keyboardCircleRadius + touchDeltaY) + keyboardLetterSquare)
         ) {         
         Tft.fillRectangle(0, 30, 200, 30, BLACK); 
         Tft.drawString("Decode = ", 10, 30, 2, WHITE);
         Tft.drawString("Encode = ", 10, 60, 2, WHITE);
         
         // Tricky : Concat 1 string and 1 char
         char charTemp[1];
         sprintf(charTemp, "%c", keyboardRow1st[letterCounterByLine]);
         strcat(message, charTemp);
         
         Tft.drawString(message, 110, 30, 2, WHITE);
         
         return keyboardRow1st[letterCounterByLine];
         }
    }





    // Test 2nd row
    
    keyboardPosY = keyboardPosY + 25;
    keyboardLeftBorderRow2nd = 25;
    keyboardCirclePosX = 32;
    keyboardCirclePosY = keyboardPosY + 7;    
    for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow2nd) - 1; letterCounterByLine++) {     
      if (p.x > (keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) && 
          p.y > (keyboardCirclePosY - keyboardCircleRadius) + touchDeltaY &&
          p.x < ((keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) + keyboardLetterSquare)  && 
          p.y < ((keyboardCirclePosY - keyboardCircleRadius + touchDeltaY) + keyboardLetterSquare)
         ) {         
         Tft.fillRectangle(0, 30, 200, 30, BLACK); 
         Tft.drawString("Decode = ", 10, 30, 2, WHITE);
         Tft.drawString("Encode = ", 10, 60, 2, WHITE);
         
         char charTemp[1];
         sprintf(charTemp, "%c", keyboardRow2nd[letterCounterByLine]);
         strcat(message, charTemp);
         
         Tft.drawString(message, 110, 30, 2, WHITE);   
         
         return keyboardRow2nd[letterCounterByLine];
         }
    }



    // Test 3rd row
    
    keyboardPosY = keyboardPosY + 25;
    keyboardLeftBorderRow1st = 15;
    keyboardCirclePosX = 22;
    keyboardCirclePosY = keyboardPosY + 7;    
    for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow3rd) - 1; letterCounterByLine++) {     
      if (p.x > (keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) && 
          p.y > (keyboardCirclePosY - keyboardCircleRadius) + touchDeltaY &&
          p.x < ((keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) + keyboardLetterSquare)  && 
          p.y < ((keyboardCirclePosY - keyboardCircleRadius + touchDeltaY) + keyboardLetterSquare)
         ) {         
         Tft.fillRectangle(0, 30, 200, 30, BLACK); 
         Tft.drawString("Decode = ", 10, 30, 2, WHITE);
         Tft.drawString("Encode = ", 10, 60, 2, WHITE);
         
         char charTemp[1];
         sprintf(charTemp, "%c", keyboardRow3rd[letterCounterByLine]);
         strcat(message, charTemp);
         
         Tft.drawString(message, 110, 30, 2, WHITE);
         
         return keyboardRow3rd[letterCounterByLine];
         }
    }  
  }
  
  return ' ';  
}



void DrawBits(int bX, int bY, byte rX, byte rY, byte px, int color)
{
  int x, y;

  x = bX + rX * 8;
  y = bY + rY;

  for (byte i = 0; i < 8; i++)
  {
    if ((px >> (7 - i)) & 0x01)
    {
      Tft.setPixel(x + i, y, color);
    }
  }
}

void DrawLogo()
{
  const char PROGMEM *logobptr = (const char PROGMEM *)LOGOBSF;
  const char PROGMEM *logoptr = (const char PROGMEM *)LOGOSF;
  byte px;
  int ndx = 0;

  for (byte y = 0; y < 25; y++)
  {
    for (byte x = 0; x < 8; x++)
    {
      px = pgm_read_byte(logobptr + ndx);
      DrawBits(175, 5, x, y, px, BLACK);

      px = pgm_read_byte(logoptr + ndx);
      DrawBits(175, 5, x, y, px, GRAY1);

      ndx++;
    }
  }
  return;
}

