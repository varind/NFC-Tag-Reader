// Adapted from Seeed Technology Inc readMifareMemory http://www.seeedstudio.com/wiki/NFC_Shield_V2.0
// Audio control from http://therandombit.wordpress.com/2011/11/21/arduino-piezo-speaker-super-mario/
// and:http://www.arduino.cc/en/Tutorial/PlayMelody
// Modified by varind (www.variableindustries.com) 2013
//
// Requires Seeed Tecnology NFC Shield V2.0
// RGB LED Cathode to Arduino grownd (via 220K Ohm).  Anodes to pins 4,5, and 6
// Piezo from ground to Arduino pin 9.


#include <PN532.h>
#include <SPI.h>


//(*0.0000001)
#define C    1911
#define C1    1804
#define D    1703
#define Eb    1607
#define E    1517
#define F    1432
#define F1    1352
#define G    1276
#define Ab    1204
#define A    1136
#define Bb    1073
#define B    1012
#define c       955
#define c1      902
#define d       851
#define eb      803
#define e       758
#define f       716
#define f1      676
#define g       638
#define ab      602
#define a       568
#define bb      536
#define b       506
#define p       0  //pause

#define red 4 // red LED pin 
#define green 3 // green LED
#define blue 2 // blue LED


int startup = 1; 
int speaker = 9;    //speaker pin
long vel = 18000;

int melody[] = {
  c, G, E, A, B, Bb, A, G, e, g, a, f, g, e, c, d, B,};
int rate[] = {
  18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 10,};
int smoke_m[] = {
  E, G, A, E, G, Bb, A, E, G, A, G, E};
int smoke_r[] = {
  12, 12, 18, 12, 12, 6, 18, 12, 12, 18, 12, 24};
int natal_m[] = {
  G, A, G, E, G, A, G, E, c, c, A, B, B, G, A, G, A, c, B, A, G, A, G, E};
int natal_r[] = {
  12, 4, 8, 16, 12, 4, 8, 16, 12, 4, 16, 12, 4, 16, 12, 4, 8, 8, 8, 8, 12, 4, 8, 16};
#define PN532_CS 10

PN532 nfc(PN532_CS);
#define  NFC_DEMO_DEBUG 1

void setup(void) {

  pinMode(red, OUTPUT);  
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(speaker, OUTPUT);
  
  digitalWrite(red, LOW); 
  digitalWrite(green, LOW); 
  digitalWrite(blue, HIGH); 

  Serial.begin(9600);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // Got ok data, print it out!

  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);
  Serial.print("Supports "); 
  Serial.println(versiondata & 0xFF, HEX);

  // configure board to read RFID tags and cards
  nfc.SAMConfig();
}

void loop(void) {

  //Play tones on startup
  if (startup == 1){ 
    play(B, 200000);
    delayMicroseconds(1000);
    play(b, 200000);
    delayMicroseconds(1000);
  }
  startup = 0;

  uint32_t id;
  // look for MiFare type cards
  id = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A);

  if (id != 0) 
  {
    Serial.print("Read card #"); 
    Serial.println(id);
    uint8_t keys[]= { 
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF         };
    if(nfc.authenticateBlock(1, id ,0x08,KEY_A,keys)) //authenticate block 0x08
    {
      //if authentication successful
      uint8_t block[16];
      //read memory block 0x08
      if(nfc.readMemoryBlock(1,0x08,block))
      {

        //if read operation is successful
        digitalWrite(green, HIGH); 
        digitalWrite(blue, LOW); 

        for(uint8_t i=0;i<16;i++)
        {
          //print memory block
          Serial.print(block[i],HEX);
          Serial.print(" ");
        }

        Serial.println();

        if (id == 2123484890){
          for (int i=0; i<17; i++) {
            int note = melody[i];
            int tempo = rate[i];
            long tvalue = tempo * vel;
            play(note, tvalue);
            delayMicroseconds(1000); // pause between notes
          }
        }

        else if(id == 2122320859){
          for (int i=0; i<24; i++) {
            int note = natal_m[i];
            int tempo = natal_r[i];
            long tvalue = tempo * vel;
            play(note, tvalue);
            delayMicroseconds(1000); // pause between notes
          }
        }

        else{
          //play success tone  
          play(g, 100000);
          delayMicroseconds(1000);
          play(a, 100000);
          delayMicroseconds(1000);
          play(b, 100000);
          delay(2000);
        }
      }
    }
    else{


      uint8_t block[4];
      //read memory block 0x08
      if(nfc.readMemoryBlock(1,0x04,block))
      {

        //if read operation is successful
        digitalWrite(green, HIGH); 
        digitalWrite(blue, LOW); 

        for(uint8_t i=0;i<4;i++)
        {
          //print memory block
          Serial.print(block[i],HEX);
          Serial.print(" ");
        }  

      }
      digitalWrite(red, HIGH); 
      digitalWrite(blue, LOW);
      play(b, 100000);
      delayMicroseconds(1000);
      play(a, 100000);
      delayMicroseconds(1000);
      play(g, 100000);
      delay(2000);
    }
  }
  delay(100);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);  
  digitalWrite(blue, HIGH); 
  //  digitalWrite(speaker, LOW);
}


void play(int note, long tempo_value) {
  long tempo_last = 0;
  while (tempo_last < tempo_value) {
    digitalWrite(speaker, HIGH);
    delayMicroseconds(note / 2);

    digitalWrite(speaker, LOW);
    delayMicroseconds(note/2);

    tempo_last += note;
  }
}


