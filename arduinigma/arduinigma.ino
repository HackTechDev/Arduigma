/*
Arduino + Enigma = Arduigma

Enigma algorithm: http://arduinoenigma.blogspot.fr/2014/10/source-code-for-implementation-of.html
*/

#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <SeeedTouchScreen.h> 

// Logo

const __FlashStringHelper * LOGOBSF;
const __FlashStringHelper * LOGOSF;

// Background 

byte BackgroundDensity = 1;

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

// Lampboard
char lampboardRow1st[] = "QWERTZUIO"; // 9
char lampboardRow2nd[] = "ASDFGHJK"; // 8
char lampboardRow3rd[] = "PYXCVBNML"; // 9


int lampboardLeftBorderRow1st;
int lampboardLeftBorderRow2nd;
int lampboardPosY;

int lampboardCirclePosX;
int lampboardCirclePosY;
int lampboardCircleRadius = 11; 
int lampboardLetterSquare = lampboardCircleRadius * 2;


// Keyboard
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


//


int labelLeftMargin = 10;
int messageLeftMargin = 50;


int inputTopMargin = 95;
int outputTopMargin = inputTopMargin + 20;

char message[20];
char messageDecode[20];

char messageChar;

// Initilization Enigma

//this example is configured as an M3 with A reflector.
//wheels 1,2,3, all starting positions set to A, no plugs

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#ifndef INT16U
#define INT16U unsigned short
#endif

// Rotor definitions, first two letters are the letter at which they rotate the one to the
// left, the rest are the output given an input letter
// rotors Beta & Gamma can only be used in the fourth position.
// the wheels go as follow:
// UKW,ROTOR,ROTOR,ROTOR,ROTOR,EKW
// M3
// byte WHEELTYPE[5] = {11, 1, 2, 3, 0, 12};
// M4 compatible with M3, set wheel 4 to A
// byte WHEELTYPE[5] = {11, 1, 2, 3, 4, 12};

//               ABCDEFGHIJKLMNOPQRSTUVWXYZ
#define ROTOR1 R-EKMFLGDQVZNTOWYHXUSPAIBRCJ
#define ROTOR2 F-AJDKSIRUXBLHWTMCQGZNPYFVOE
#define ROTOR3 W-BDFHJLCPRTXVZNYEIWGAKMUSQO
#define ROTOR4 K-ESOVPZJAYQUIRHXLNFTGKDCMWB
#define ROTOR5 A-VZBRGITYUPSDNHLXAWMJQOFECK
#define ROTOR6 ANJPGVOUMFYQBENHZRDKASXLICTW
#define ROTOR7 ANNZJHGRCXMYSWBOUFAIVLPEKQDT
#define ROTOR8 ANFKQHTLXOCBJSPDZRAMEWNIUYGV
#define ROTORB --LEYJVCNIXWPBQMDRTAKZGFUHOS
#define ROTORG --FSOKANUERHMBTIYCWLQPZXVGJD
#define    ETW --ABCDEFGHIJKLMNOPQRSTUVWXYZ
#define   UKWA --EJMZALYXVBWFCRQUONTSPIKHGD
#define   UKWB --YRUHQSLDPXNGOKMIEBFZCWVJAT
#define   UKWC --FVPJIAOYEDRZXWGCTKUQSBNMHL
#define  UKWBD --ENKQAUYWJICOPBLMDXZVFTHRGS
#define  UKWCD --RDOBJNTKVEHMLFCWZAXGYIPSUQ

const __FlashStringHelper * WHEELSF;
const __FlashStringHelper * UHRSF;
const __FlashStringHelper * UHRPLUGSF;

char EffSTECKER[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
byte WHEELPOS[4] = {'A', 'A', 'A', 'A'};

// wheel types, first entry is for rightmost wheel, last is reflector
// fifth entry can be 0 for a three rotor machine.
//byte WHEELTYPE[6] = {11, 3, 2, 1, 0, 12};
//byte WHEELTYPE[6] = {11, 3, 2, 1, 9, 15};

//default M3 1939 in enigma simulator app
//byte WHEELTYPE[6] = {11, 3, 2, 1, 0, 12};
//byte WHEELPOS[4] = {0, 'A', 'A', 'A'};

//default M4 1942 in enigma simulator app
//byte WHEELTYPE[6] = {11, 3, 2, 1, 9, 15};
//byte WHEELPOS[4] = {'A', 'A', 'A', 'A'};

// test machine
//byte WHEELTYPE[6] = {11, 1, 1, 1, 1, 15};
//byte WHEELPOS[4] = {'A', 'A', 'A', 'A'};

char KeyPressed = 0;
char EncodedKey = 0;

byte SerialRead = 0;
byte SerialSetWheels = 0;

struct enigmaData_t
{
  byte SerialFunction;
  byte Uhr;
  char PAIRS[27];
  char STECKER[27];
  byte WHEELTYPE[6];
  byte WHEELPOS[4];
  byte ROTORPOS[4];
}
EnigmaData;


void initEnigma()
{
  // set to 1 to monitor or 2 to display encoded character only
  EnigmaData.SerialFunction = 2;
  
  //ALL THE STEPS SHOWN BELOW MUST BE DONE TO INITIALIZE THE MACHINE
  
  //Rotor Types for an M4 machine, see more examples above
  EnigmaData.WHEELTYPE[0] = 11;    // ENTRY CONTACTS: ETW
  EnigmaData.WHEELTYPE[1] = 3;     // RIGHTMOST ROTOR
  EnigmaData.WHEELTYPE[2] = 2;     // MIDDLE ROTOR
  EnigmaData.WHEELTYPE[3] = 1;     // LEFT ROTOR
  EnigmaData.WHEELTYPE[4] = 0;     // ADDITIONAL WHEEL (M4 only)
  EnigmaData.WHEELTYPE[5] = 12;    // REFLECTOR: UKW

  //Wheel Positions
  EnigmaData.WHEELPOS[0] = 0;      // LEFTMOST LETTER ON M4
  EnigmaData.WHEELPOS[1] = 'A';    // LEFTMOST LETTER ON M3
  EnigmaData.WHEELPOS[2] = 'A';    // MIDDLE LETTER
  EnigmaData.WHEELPOS[3] = 'A';    // RIGHTMOST LETTER

  //Ring Settings
  EnigmaData.ROTORPOS[0] = 0;           // LEFTMOST ROTOR SETTING ON M4
  EnigmaData.ROTORPOS[1] = 'A'-64;      // LEFTMOST ROTOR SETTING ON M3
  EnigmaData.ROTORPOS[2] = 'A'-64;      // MIDDLE ROTOR SETTING
  EnigmaData.ROTORPOS[3] = 'A'-64;      // RIGHTMOST ROTOR SETTING

  for (byte i = 0; i < 4; i++)
  {
    WHEELPOS[i] = EnigmaData.WHEELPOS[i];
  }

  //Initialize stecker with no plugs
  RemoveAllPlugs();
  
  //ADD ANY PLUG PAIRS HERE USING THE AddPlug function
  //AddPlug('E', 'Q');
  //AddPlug('N', 'R');
  //AddPlug('I', 'S');
  //AddPlug('G', 'T');
  //AddPlug('M', 'U');
  //AddPlug('A', 'V');

  // set the UHR (non zero values used only if 10 plug pairs are installed)
  EnigmaData.Uhr = 0;
  CalculateUhrStecker();
}



void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:    
  
  // Enigma
  
  
  WHEELSF = F(STR(ROTOR1) STR(ROTOR2) STR(ROTOR3) STR(ROTOR4) STR(ROTOR5) STR(ROTOR6) STR(ROTOR7) STR(ROTOR8) STR(ROTORB) STR(ROTORG) STR(ETW) STR(UKWA) STR(UKWB) STR(UKWC) STR(UKWBD) STR(UKWCD));
  UHRSF = F("\x06\x1F\x04\x1D\x12\x27\x10\x19\x1E\x17\x1C\x01\x26\x0B\x24\x25\x1A\x1B\x18\x15\x0E\x03\x0C\x11\x02\x07\x00\x21\x0A\x23\x08\x05\x16\x13\x14\x0D\x22\x0F\x20\x09");
  UHRPLUGSF = F("\x06\x00\x07\x05\x01\x08\x04\x02\x09\x03");

  initEnigma();
  ShowRotors();
  
  // TFT
  TFT_BL_ON;      // turn on the background light
  Tft.TFTinit();  // init TFT library
   

   
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

  //PaintBackground(0, 0, 0, 0);

  Tft.drawString("Arduigma", 0, 0, 2, WHITE); 
  
  DrawLogo();
  
    Tft.drawLine(0, inputTopMargin - 10, 240, inputTopMargin - 10, WHITE);
    

    Tft.fillRectangle(0, inputTopMargin, 240, 30, BLACK); 
    Tft.drawString("I = ", labelLeftMargin, inputTopMargin, 2, WHITE);     
  
    Tft.fillRectangle(0, outputTopMargin, 240, 15, BLACK);    
     Tft.drawString("O = ", labelLeftMargin, outputTopMargin, 2, WHITE);
         
         
         
    Tft.drawLine(0, 135, 240, 135, WHITE);
    
  // Lampboard 1st row
  
    
  lampboardPosY = 150;
  lampboardLeftBorderRow1st = 15;
  lampboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow1st) - 1; letterCounterByLine++) {

    lampboardCirclePosY = lampboardPosY + 7;
    Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, BLACK);
    Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
    Tft.drawChar(lampboardRow1st[letterCounterByLine], lampboardLeftBorderRow1st + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);
    //Tft.drawRectangle(lampboardCirclePosX + (letterCounterByLine * 25) - lampboardCircleRadius, lampboardCirclePosY - lampboardCircleRadius, lampboardCircleRadius * 2, lampboardCircleRadius * 2, BLUE);
  }
  
  
   // Lampboard 2nd row
  
  lampboardPosY = lampboardPosY + 25;
  lampboardLeftBorderRow2nd = 25;
  lampboardCirclePosX = 32;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow2nd) - 1; letterCounterByLine++) {

    lampboardCirclePosY = lampboardPosY + 7;
    Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, BLACK);    
    Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
    Tft.drawChar(lampboardRow2nd[letterCounterByLine], lampboardLeftBorderRow2nd + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);    
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  } 
   
  
  // Lampboard 3rd row
  
  lampboardPosY = lampboardPosY + 25;
  lampboardLeftBorderRow1st = 15;
  lampboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow3rd) - 1; letterCounterByLine++) {
    lampboardCirclePosY = lampboardPosY + 7;
    Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, BLACK);    
    Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
    Tft.drawChar(lampboardRow3rd[letterCounterByLine], lampboardLeftBorderRow1st + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  } 
  
  
  Tft.drawLine(0, 230, 240, 230, WHITE);
  

   
  // Keyboard 1st row
  
  keyboardPosY = 245;
  keyboardLeftBorderRow1st = 15;
  keyboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow1st) - 1; letterCounterByLine++) {

    keyboardCirclePosY = keyboardPosY + 7;
    Tft.fillCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, BLACK);
    Tft.drawCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, WHITE);
    Tft.drawChar(keyboardRow1st[letterCounterByLine], keyboardLeftBorderRow1st + (letterCounterByLine * 25), keyboardPosY, 2, WHITE);
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  }
   
  // Keyboard 2nd row
  
  keyboardPosY = keyboardPosY + 25;
  keyboardLeftBorderRow2nd = 25;
  keyboardCirclePosX = 32;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow2nd) - 1; letterCounterByLine++) {

    keyboardCirclePosY = keyboardPosY + 7;
    Tft.fillCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, BLACK);    
    Tft.drawCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, WHITE);
    Tft.drawChar(keyboardRow2nd[letterCounterByLine], keyboardLeftBorderRow2nd + (letterCounterByLine * 25), keyboardPosY, 2, WHITE);    
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  } 
   
  
  // Keyboard 3rd row
  
  keyboardPosY = keyboardPosY + 25;
  keyboardLeftBorderRow1st = 15;
  keyboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow3rd) - 1; letterCounterByLine++) {
    keyboardCirclePosY = keyboardPosY + 7;
    Tft.fillCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, BLACK);    
    Tft.drawCircle(keyboardCirclePosX + (letterCounterByLine * 25), keyboardCirclePosY, keyboardCircleRadius, WHITE);
    Tft.drawChar(keyboardRow3rd[letterCounterByLine], keyboardLeftBorderRow1st + (letterCounterByLine * 25), keyboardPosY, 2, WHITE);
    //Tft.drawRectangle(keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius, keyboardCirclePosY - keyboardCircleRadius, keyboardCircleRadius * 2, keyboardCircleRadius * 2, BLUE);
  }


}

void loop() {
  
  messageChar = detectButtonLetter();
  
  if(messageChar != ' ' ) {
    Serial.println(messageChar);
    
    
    
    
   KeyPressed = messageChar;

    if (KeyPressed == '!')
    {
      if (EnigmaData.WHEELTYPE[4] == 0)
      {
        SerialSetWheels = 3;
      }
      else
      {
        SerialSetWheels = 4;
      }
    }

    KeyPressed = KeyPressed & (255 - 32);

    if ((KeyPressed > 'A' - 1) && (KeyPressed < 'Z' + 1))
    {
      SerialRead = 1;
      if (SerialSetWheels)
      {
        if (WHEELPOS[4 - SerialSetWheels] != KeyPressed)
        {
          WHEELPOS[4 - SerialSetWheels] = KeyPressed;
        }
        SerialSetWheels--;
      }
      else
      {
        MoveWheels();
        EncodedKey = EncodeKey(KeyPressed);
        if (EnigmaData.SerialFunction == 2)
        {
         Serial.print(EncodedKey);
         Tft.fillRectangle(0, outputTopMargin, 240, 15, BLACK);    
         Tft.drawString("O = ", labelLeftMargin, outputTopMargin, 2, WHITE);
 
         char charTemp[1];
         sprintf(charTemp, "%c", EncodedKey);
         strcat(messageDecode, charTemp);
         
         Tft.drawString(messageDecode, messageLeftMargin, outputTopMargin, 2, WHITE);  
        
        lampOn(EncodedKey); 
        ShowRotors();
        }
      }
    }    
    
    
  }
  
  delay(100);  
}


void lampOn(char letter) {
  //Serial.print(letter);
  lampOff();
    
  // Lampboard 1st row
  
    
  lampboardPosY = 150;
  lampboardLeftBorderRow1st = 15;
  lampboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow1st) - 1; letterCounterByLine++) {

    lampboardCirclePosY = lampboardPosY + 7;
    
    if(lampboardRow1st[letterCounterByLine] == letter) {
 
      Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, RED);
      Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
      Tft.drawChar(lampboardRow1st[letterCounterByLine], lampboardLeftBorderRow1st + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);
    }
  }
  
  
   // Lampboard 2nd row
  
  lampboardPosY = lampboardPosY + 25;
  lampboardLeftBorderRow2nd = 25;
  lampboardCirclePosX = 32;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow2nd) - 1; letterCounterByLine++) {

    lampboardCirclePosY = lampboardPosY + 7;
    if(lampboardRow2nd[letterCounterByLine] == letter) {

      Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, RED);    
      Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
      Tft.drawChar(lampboardRow2nd[letterCounterByLine], lampboardLeftBorderRow2nd + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);  
    }  
  } 
   
  
  // Lampboard 3rd row
  
  lampboardPosY = lampboardPosY + 25;
  lampboardLeftBorderRow1st = 15;
  lampboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow3rd) - 1; letterCounterByLine++) {
    lampboardCirclePosY = lampboardPosY + 7;
    if(lampboardRow3rd[letterCounterByLine] == letter) {
     
      Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, RED);    
      Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
      Tft.drawChar(lampboardRow3rd[letterCounterByLine], lampboardLeftBorderRow1st + (letterCounterByLine * 25), lampboardPosY, 2, WHITE); 
    }
  } 
  
}

// TODO: Do not turn on all the light but only one
void lampOff() {
    
  // Lampboard 1st row
  
    
  lampboardPosY = 150;
  lampboardLeftBorderRow1st = 15;
  lampboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow1st) - 1; letterCounterByLine++) {

    lampboardCirclePosY = lampboardPosY + 7;
    Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, BLACK);
    Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
    Tft.drawChar(lampboardRow1st[letterCounterByLine], lampboardLeftBorderRow1st + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);
  }
  
   // Lampboard 2nd row
  
  lampboardPosY = lampboardPosY + 25;
  lampboardLeftBorderRow2nd = 25;
  lampboardCirclePosX = 32;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow2nd) - 1; letterCounterByLine++) {

    lampboardCirclePosY = lampboardPosY + 7;
    Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, BLACK);    
    Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
    Tft.drawChar(lampboardRow2nd[letterCounterByLine], lampboardLeftBorderRow2nd + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);    
  } 
   
  // Lampboard 3rd row
  
  lampboardPosY = lampboardPosY + 25;
  lampboardLeftBorderRow1st = 15;
  lampboardCirclePosX = 22;
  for (letterCounterByLine = 0; letterCounterByLine < sizeof(lampboardRow3rd) - 1; letterCounterByLine++) {
    lampboardCirclePosY = lampboardPosY + 7;
    Tft.fillCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, BLACK);    
    Tft.drawCircle(lampboardCirclePosX + (letterCounterByLine * 25), lampboardCirclePosY, lampboardCircleRadius, WHITE);
    Tft.drawChar(lampboardRow3rd[letterCounterByLine], lampboardLeftBorderRow1st + (letterCounterByLine * 25), lampboardPosY, 2, WHITE);
  } 
}


char detectButtonLetter() {

  Point p = ts.getPoint();

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);


  
  if (p.z > __PRESURE) {

   Serial.print("presureX = "); Serial.print(p.x); Serial.print(" presureY = "); Serial.println(p.y);  
   

    // Test 1st row
    keyboardPosY = 245;
    keyboardLeftBorderRow1st = 15;
    keyboardCirclePosX = 22;    
    keyboardCirclePosY = keyboardPosY + 7;    
    for (letterCounterByLine = 0; letterCounterByLine < sizeof(keyboardRow1st) - 1; letterCounterByLine++) {     
      if (p.x > (keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) && 
          p.y > (keyboardCirclePosY - keyboardCircleRadius) + touchDeltaY &&
          p.x < ((keyboardCirclePosX + (letterCounterByLine * 25) - keyboardCircleRadius) + keyboardLetterSquare)  && 
          p.y < ((keyboardCirclePosY - keyboardCircleRadius + touchDeltaY) + keyboardLetterSquare)
         ) {         
         Tft.fillRectangle(0, inputTopMargin, 240, 30, BLACK); 
         Tft.drawString("I = ", labelLeftMargin, inputTopMargin, 2, WHITE);
         
         // Tricky : Concat 1 string and 1 char
         char charTemp[1];
         sprintf(charTemp, "%c", keyboardRow1st[letterCounterByLine]);
         strcat(message, charTemp);
         
         Tft.drawString(message, messageLeftMargin, inputTopMargin, 2, WHITE);
         
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
         Tft.fillRectangle(0, inputTopMargin, 240, 30, BLACK); 
         Tft.drawString("O = ", labelLeftMargin, inputTopMargin, 2, WHITE);

         
         char charTemp[1];
         sprintf(charTemp, "%c", keyboardRow2nd[letterCounterByLine]);
         strcat(message, charTemp);
         
         Tft.drawString(message, messageLeftMargin, inputTopMargin, 2, WHITE);   
         
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
         Tft.fillRectangle(0, inputTopMargin, 240, 30, BLACK); 
         Tft.drawString("O = ", labelLeftMargin, inputTopMargin, 2, WHITE);

         
         char charTemp[1];
         sprintf(charTemp, "%c", keyboardRow3rd[letterCounterByLine]);
         strcat(message, charTemp);
         
         Tft.drawString(message, messageLeftMargin, inputTopMargin, 2, WHITE);
         
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


byte SteckerPairs()
{
  byte c = 0;

  for (byte i = 0; i < 26; i++)
  {
    if (EnigmaData.STECKER[i] != (65 + i))
    {
      c++;
    }
  }

  return (c / 2);
}

void RemoveAllPlugs()
{
  for (byte i = 0; i < 27; i++)
  {
    EnigmaData.PAIRS[i] = 0;
  }

  strcpy(EnigmaData.STECKER, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  EnigmaData.Uhr = 0;
}

void AddPlug(char PlugKey1, char PlugKey2)
{
  if ((SteckerPairs() < 13) && (PlugKey1 != PlugKey2))
  {
    EnigmaData.PAIRS[SteckerPairs() * 2] = PlugKey1;
    EnigmaData.PAIRS[SteckerPairs() * 2 + 1] = PlugKey2;
  }

  EnigmaData.STECKER[PlugKey1 - 65] = PlugKey2;
  EnigmaData.STECKER[PlugKey2 - 65] = PlugKey1;
}

void CalculateUhrStecker()
{
  const char PROGMEM *uhrptr = (const char PROGMEM *)UHRSF;
  const char PROGMEM *uhrplugptr = (const char PROGMEM *)UHRPLUGSF;

  byte ndx = 0;

  if (SteckerPairs() == 10)
  {
    for (byte i = 0; i < 26; i++)
    {
      EffSTECKER[i] = 65 + i;
    }

    for (byte i = 0; i < 10; i++)
    {
      byte pin = 0;
      byte pinright = 0;
      byte pinleft = 0;

      pin = EnigmaData.Uhr + i * 4;
      if (pin > 39)
      {
        pin -= 40;
      }

      for (byte j = 0; j < 40; j++)
      {
        if (pgm_read_byte(uhrptr + j) == pin)
        {
          pinleft = j;
        }
      }

      pinright = pgm_read_byte(uhrptr + pin);

      //these two need to be signed, see <0 below
      //char is signed -127..+128
      char plugright;
      char plugleft;

      plugright = (pinright - (EnigmaData.Uhr + 2));
      if (plugright < 0)
      {
        plugright += 40;
      }
      plugright = plugright / 4;

      plugleft = (pinleft - (EnigmaData.Uhr + 2));
      if (plugleft < 0)
      {
        plugleft += 40;
      }
      plugleft = plugleft / 4;

      EffSTECKER[EnigmaData.PAIRS[i * 2] - 65] = EnigmaData.PAIRS[pgm_read_byte(uhrplugptr + plugright) * 2 + 1];
      EffSTECKER[EnigmaData.PAIRS[pgm_read_byte(uhrplugptr + i) * 2 + 1] - 65] = EnigmaData.PAIRS[plugleft * 2];
    }
  }
  else
  {
    for (byte i = 0; i < 26; i++)
    {
      EffSTECKER[i] = EnigmaData.STECKER[i];
    }
  }

  if (EnigmaData.SerialFunction == 1)
  {
    Serial.println(F("Stecker/Uhr:"));
    Serial.println(EffSTECKER);
  }
}

void MoveWheels()
{
  byte i = 4;
  bool carry = true;

  do
  {
    i--;

    if (carry)
    {
      WHEELPOS[i]++;
      if (i > 1)
      {
        carry = IsCarry(EnigmaData.WHEELTYPE[4 - i], WHEELPOS[i]);
      }
      else
      {
        carry = false;
      }
    }
    else
    {
      // double stepping on second wheel
      if (i == 2)
      {
        byte w2 = WHEELPOS[2] + 1;

        if (w2 > 'Z')
        {
          w2 = 'A';
        }

        if (IsCarry(EnigmaData.WHEELTYPE[2], w2))
        {
          WHEELPOS[2]++;
          carry = true;
        }
      }
    }

    if (WHEELPOS[i] > 'Z')
    {
      WHEELPOS[i] = 'A';
      carry = IsCarry(EnigmaData.WHEELTYPE[4 - i], WHEELPOS[i]) || carry;
      if (i == 1)
      {
        carry = false;
      }
    }

  } while (i > 0);

}

void ShowRotors()
{
  const char PROGMEM *charptr = (const char PROGMEM *)WHEELSF;
  char k;
  INT16U wheeltype;

 
    Serial.println(F("Rotors:"));

    for (byte i = 0; i < 6; i++)
    {
      if (EnigmaData.WHEELTYPE[i] != 0)
      {
        switch (i)
        {
          case 0:
            {
              Serial.println(F("ETW"));
              break;
            }

          case 1:
          case 2:
          case 3:
          case 4:
            {
              Serial.print(F("R"));
              Serial.println((char)('0' + i));
              break;
            }

          case 5:
            {
              Serial.println(F("UKW"));
              break;
            }
        }

        wheeltype = ((EnigmaData.WHEELTYPE[i] - 1) * 28) + 2;

        for (byte i = 0; i < 26; i++)
        {
          k = pgm_read_byte(charptr + wheeltype + i);
          Serial.print(k);
        }
        Serial.println(F(""));
      }
    }
 
}

byte SerialMonitorStatus;

void SerialMonitor(char k)
{
  if (k == 0)
  {
    SerialMonitorStatus = 0;
  }
  else
  {

      SerialMonitorStatus++;

      //skip R4 if a three wheel machine
      if (((EnigmaData.WHEELTYPE[5] > 11) && (EnigmaData.WHEELTYPE[5] < 15)) && ((SerialMonitorStatus == 6) || (SerialMonitorStatus == 8)))
      {
        SerialMonitorStatus++;
      }

      Serial.print(k);

      switch (SerialMonitorStatus)
      {
        case 1:
        case 13:
          {
            Serial.print(F(">Stecker>"));
            break;
          }

        case 2:
        case 12:
          {
            Serial.print(F(">ETW>"));
            break;
          }

        case 3:
        case 11:
          {
            Serial.print(F(">R1>"));
            break;
          }

        case 4:
        case 10:
          {
            Serial.print(F(">R2>"));
            break;
          }

        case 5:
        case 9:
          {
            Serial.print(F(">R3>"));
            break;
          }

        case 6:
        case 8:
          {
            Serial.print(F(">R4>"));
            break;
          }

        case 7:
          {
            Serial.print(F(">UKW>"));
            break;
          }
        default:
          {
            Serial.println(F(""));
          }
      }
    
  }
}

bool IsCarry(byte wheelType, byte wheelPos)
{
  const char PROGMEM *charptr = (const char PROGMEM *)WHEELSF;
  INT16U wheeltype = (wheelType - 1) * 28;
  byte k1 = pgm_read_byte(charptr + wheeltype);
  byte k2 = pgm_read_byte(charptr + wheeltype + 1);
  bool v = false;

  if ((wheelPos == k1) || (wheelPos == k2))
  {
    v = true;
  }

  return v;
}

char EncodeKey(char key)
{
  const char PROGMEM *charptr = (const char PROGMEM *)WHEELSF;
  char k, k1;
  INT16U wheeltype;

  SerialMonitor(0);
  SerialMonitor(key);

  k = EffSTECKER[key - 'A'];

  SerialMonitor(k);

  for (byte i = 0; i < 6; i++)
  {
    if (EnigmaData.WHEELTYPE[i] != 0)
    {
      if ((i > 0) && (i < 5))
      {
        byte p = WHEELPOS[4 - i] - (EnigmaData.ROTORPOS[4 - i] - 1);
        if (p < 'A')
        {
          p += 26;
        }

        k = k + (p - 'A');
      }

      if (k > 'Z')
      {
        k = k - ('Z' + 1);
      }
      else
      {
        k = k - 'A';
      }

      wheeltype = ((EnigmaData.WHEELTYPE[i] - 1) * 28) + k + 2;
      k = pgm_read_byte(charptr + wheeltype);

      if ((i > 0) && (i < 5))
      {
        byte p = WHEELPOS[4 - i] - (EnigmaData.ROTORPOS[4 - i] - 1);
        if (p < 'A')
        {
          p += 26;
        }

        k = k - (p - 'A');
      }

      if (k < 'A')
      {
        k = k + 26;
      }

      SerialMonitor(k);
    }
  }

  //after reflector

  for (byte i = 0; i < 5; i++)
  {
    if (EnigmaData.WHEELTYPE[4 - i] != 0)
    {
      if (i < 4)
      {
        byte p = WHEELPOS[i] - (EnigmaData.ROTORPOS[i] - 1);
        if (p < 'A')
        {
          p += 26;
        }

        k = k + (p - 'A');
      }

      if (k > 'Z')
      {
        k = k - 26;
      }

      wheeltype = (EnigmaData.WHEELTYPE[4 - i] - 1) * 28;
      for (byte j = 0; j < 26; j++)
      {
        if ((pgm_read_byte(charptr + wheeltype + j + 2)) == k)
        {
          k1 = 'A' + j;
        }
      }

      k = k1;

      if (i < 4)
      {
        byte p = WHEELPOS[i] - (EnigmaData.ROTORPOS[i] - 1);
        if (p < 'A')
        {
          p += 26;
        }

        k = k - (p - 'A');
      }

      if (k < 'A')
      {
        k = k + 26;
      }

      SerialMonitor(k);
    }

  }

  for (byte j = 0; j < 26; j++)
  {
    if (EffSTECKER[j] == k)
    {
      k1 = 'A' + j;
    }
  }
  k = k1;

  SerialMonitor(k);

  return k;
}

// Background 

void PaintBackground(int x1, int y1, int x2, int y2)
{
  if (BackgroundDensity == 0)
  {
    return;
  }

  int X1 = x1;
  int Y1 = y1;
  int X2 = x2;
  int Y2 = y2;

  int c1 = 6339;
  int c2 = 8484;
  int c3 = 14823;

  if ((x1 == 0) && (y1 == 0) && (x2 == 0) && (y2 == 0))
  {
    X1 = 0;
    Y1 = 0;
    X2 = 239;
    Y2 = 302;
  }

  for (int j = Y1; j < Y2 + 1; j += BackgroundDensity)
  {
    for (int i = X1; i < X2 + 1; i += BackgroundDensity)
    {
      int c;
      int r;
      r = random(100);

      if (r < 30)
      {
        c = c1;
      }

      if ((r > 29) && (r < 60))
      {
        c = c2;
      }

      if (r > 59)
      {
        c = c3;
      }

      Tft.setPixel(i, j, c);
    }
  }
}

