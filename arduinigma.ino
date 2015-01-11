/*  draw text's APP
    drawChar(INT8U ascii,INT16U poX, INT16U poY,INT16U size, INT16U fgcolor);
    drawString(char *string,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
*/

#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <SeeedTouchScreen.h> 

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

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:    
   
  TFT_BL_ON;      // turn on the background light
  Tft.TFTinit();  // init TFT library
   
  Tft.drawString("Arduigma", 0, 0, 2, WHITE); 
   

   
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
  detectButtonLetter() ;
 
  delay(100);  
}

void detectButtonLetter() {

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
         }
    }



    
    
  }

}
