// Buffer para almacenar los textos de control
#include <stdlib.h>
#define BUFFSIZE 255 
//#include <SoftwareSerial.h>
#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 4 // LI
#define datapin 11 // DI
 
#define NumLEDs 2
 
int LEDChannels[NumLEDs][3] = {0};
int SB_CommandMode;
int SB_RedCommand;
int SB_GreenCommand;
int SB_BlueCommand;

int redVal = 0;
int greenVal = 0;
int blueVal = 0;
void SB_SendPacket() {
 
    if (SB_CommandMode == B01) {
     SB_RedCommand = 120;
     SB_GreenCommand = 100;
     SB_BlueCommand = 100;
    }
 
    SPDR = SB_CommandMode << 6 | SB_BlueCommand>>4;
    while(!(SPSR & (1<<SPIF)));
    SPDR = SB_BlueCommand<<4 | SB_RedCommand>>6;
    while(!(SPSR & (1<<SPIF)));
    SPDR = SB_RedCommand << 2 | SB_GreenCommand>>8;
    while(!(SPSR & (1<<SPIF)));
    SPDR = SB_GreenCommand;
    while(!(SPSR & (1<<SPIF)));
 
}
 
void WriteLEDArray() {
 
    SB_CommandMode = B00; // Write to PWM control registers
    for (int h = 0;h<NumLEDs;h++) {
	  SB_RedCommand = LEDChannels[h][0];
	  SB_GreenCommand = LEDChannels[h][1];
	  SB_BlueCommand = LEDChannels[h][2];
	  SB_SendPacket();
    }
 
    delayMicroseconds(15);
    digitalWrite(latchpin,HIGH); // latch data into registers
    delayMicroseconds(15);
    digitalWrite(latchpin,LOW);
 
    SB_CommandMode = B01; // Write to current control registers
    for (int z = 0; z < NumLEDs; z++) SB_SendPacket();
    delayMicroseconds(15);
    digitalWrite(latchpin,HIGH); // latch data into registers
    delayMicroseconds(15);
    digitalWrite(latchpin,LOW);
 
}

char buffer[BUFFSIZE];
int i = 0;
 
void flush_buffer() {
  Serial.write((const uint8_t*)buffer,i);
  for(int j=0;j<=i;j++) {
    buffer[j] = 0;
  }
  i = 0;
}
 
void append_buffer(char c) {
  if(i<BUFFSIZE) {
    buffer[i++] = c;
  } else {
    flush_buffer();
  }
}

void setup() {
 
   //mySerial.begin(9600);
   Serial.begin(9600);
  /*
   pinMode(datapin, OUTPUT);
   pinMode(latchpin, OUTPUT);
   pinMode(enablepin, OUTPUT);
   pinMode(clockpin, OUTPUT);
   SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
   digitalWrite(latchpin, LOW);
   digitalWrite(enablepin, LOW);
   
   updateColor(); */
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(11, OUTPUT);
   
   analogWrite(9, 255);
   analogWrite(10, 255);
   analogWrite(11, 255);
}

void updateColor() {
  LEDChannels[0][0] = redVal;
  LEDChannels[0][1] = greenVal;
  LEDChannels[0][2] = blueVal;
  LEDChannels[1][0] = redVal;
  LEDChannels[1][1] = greenVal;
  LEDChannels[1][2] = blueVal;
  WriteLEDArray();
}
 
 
void loop() {
  
  if (Serial.available()) {
    char c = Serial.read();
    append_buffer(c);
    Serial.write(c);
    char *str;
    if(c=='\r'||c=='\n') {
       if((str=strstr(buffer,"r"))!=0 && (strlen(str)>2)) {
         redVal = map(atoi((char*)(str+1)), 0, 1023, 0, 255);
         Serial.print("Red");
         Serial.print(redVal,DEC);
         //updateColor();
         analogWrite(9, 255-redVal);
       }
       if((str=strstr(buffer,"g"))!=0 && (strlen(str)>2)) {
         greenVal = map(atoi((char*)(str+1)), 0, 1023, 0, 255);
         Serial.print("Green");
         Serial.print(greenVal,DEC);
         //updateColor();
         analogWrite(10, 255-greenVal);
       }
       if((str=strstr(buffer,"b"))!=0 && (strlen(str)>2)) {
         blueVal = map(atoi((char*)(str+1)), 0, 1023, 0, 255);
         Serial.print("Blue");
         Serial.print(blueVal,DEC);
         //updateColor();
         analogWrite(11, 255-blueVal);
       }
       flush_buffer();
    }
  }
  //if (Serial.available())
  //  mySerial.write(Serial.read());
}
