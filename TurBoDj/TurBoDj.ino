#include <Encoder.h>
#include <Bounce2.h>

Encoder myEnc(0, 1);

#define BUTTON_PIN 23

Bounce button0 = Bounce(); 
Bounce button1 = Bounce(); 

int wheelChannel = 1;

int channel = 1;

const int latchPin = 21; //Pin connected to ST_CP of 74HC595
const int clockPin = 20; //Pin connected to SH_CP of 74HC595
const int dataPin = 22;  //Pin connected to DS of 74HC595

const int s0 = 26; //Pin connected to S0 of 4051
const int s1 = 25; //Pin connected to S1 of 4051
const int s2 = 24; //Pin connected to S2 of 4051

int leds = 0;


void setup() {

  //Serial.begin(115200);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  button0.attach(BUTTON_PIN);
  button0.interval(5); // interval in ms
  button1.attach(BUTTON_PIN);
  button1.interval(5); // interval in ms
  
  //set pins to output shift register 595
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  //set pins to output multiplexor 4051
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  
  
  wheelChannel = 1;
  leds = 0xFF;
  writeLeds(leds);
  delay(250);
  leds = 0x00;
  writeLeds(leds);
  delay(250);
  
  bitSet(leds, 0);
  writeLeds(leds);
}

long oldPosition  = -999;

void loop() {

  
  // select 74HC4051 channel 0 (of 0 to 7)
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);
  
  button0.update();

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);

  button1.update();
  
  
  if (button0.fell()) {
    usbMIDI.sendNoteOn(60, 127, wheelChannel);

  }
  
  if (button0.rose()) {
    usbMIDI.sendNoteOff(60, 0, wheelChannel);
  }
  
  if (button1.fell()) {
    if (wheelChannel == 1){
      wheelChannel = 2;
      bitSet(leds, 5);
      bitClear(leds, 0);
      writeLeds(leds);
    }
    else{
      wheelChannel = 1;
      bitSet(leds, 0);
      bitClear(leds, 5);
      writeLeds(leds);
    }

  }




  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    //Serial.println(newPosition);
    if (oldPosition < newPosition){
      usbMIDI.sendControlChange(28,0,wheelChannel);
    }
    else if (oldPosition > newPosition){
      usbMIDI.sendControlChange(28,1,wheelChannel);
    }
    oldPosition = newPosition;
  }
  usbMIDI.read();
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  /*
    Serial.print("Note On, ch=");
   Serial.print(channel, DEC);
   Serial.print(", note=");
   Serial.print(note, DEC);
   Serial.print(", velocity=");
   Serial.print(velocity, DEC);
   Serial.println();*/
  if (note == 0){
    bitSet(leds, 0);
  }
  else if (note == 1){
    bitSet(leds, 1);
  }
  else if (note == 2){
    bitSet(leds, 2);
  }
  else if (note == 21){
    bitSet(leds, 3);
  }
  else if (note == 22) {
    bitSet(leds, 4);
  }
  else if (note == 99) {
    wheelChannel = 1;
    leds = 0xFF;
    writeLeds(leds);
    delay(250);
    leds = 0x00;
  }
  writeLeds(leds);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  /*
    Serial.print("Note Off, ch=");
   Serial.print(channel, DEC);
   Serial.print(", note=");
   Serial.print(note, DEC);
   Serial.print(", velocity=");
   Serial.print(velocity, DEC);
   Serial.println();*/


  if (note == 0){
    bitClear(leds, 0);
  }
  else if (note == 1){
    bitClear(leds, 1);
  }
  else if (note == 2){
    bitClear(leds, 2);
  }
  else if (note == 21){
    bitClear(leds, 3);
  }
  else if (note == 22) {
    bitClear(leds, 4);
  }
  writeLeds(leds);
}

void writeLeds(int value){

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);  // shift out the bits:
  digitalWrite(latchPin, HIGH); //take the latch pin high so the LEDs will light up: 

}









