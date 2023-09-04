/*
  Turns a raspberry pi pico into a cnc pendant.  Was originally written
 to interface with cncjs-pendant-rpi  

To compile be sure to get the arduino mbed os rp2040 board in board manager
Also you need to select raspberry pi pico from Arduino mbed os boards
I am using arduino ide version 2.2.1
 
 In loving memory of my father.  The man who inspired me to work with 
 machines.  Ron Schwab
 */
#include "hardware/pio.h"
#include "quadrature.pio.h"

#define QUADRATURE_A_PIN 13
#define QUADRATURE_B_PIN 14
#include <PicoGamepad.h>

PicoGamepad pendant;

// 16 bit integer for holding input values
int val;
int16_t encoder_value=60;
uint ctr=0;
uint ctr2=0;
int pencoder=0;
char pen=0;
uint multiplier=0;
bool wentlow=false;
PIO pio = pio0;
uint offset, sm;

void setup() {  
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // X Potentiometer on pin 26
  pinMode(26, INPUT);
  // Y Potentiometer on pin 27
  pinMode(27, INPUT);
  pinMode(QUADRATURE_A_PIN,INPUT_PULLUP);
  pinMode(QUADRATURE_B_PIN,INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  offset = pio_add_program(pio, &quadrature_program);
  sm = pio_claim_unused_sm(pio, true);
  quadrature_program_init(pio, sm, offset, QUADRATURE_A_PIN, QUADRATURE_B_PIN);
}

void loop() {
  char en=digitalRead(QUADRATURE_A_PIN);
  char en2=digitalRead(QUADRATURE_B_PIN);
  if(wentlow){
    if(en==HIGH){
      encoder_value+=multiplier;
      wentlow=false;
    }
    else if(en2==HIGH){
      encoder_value-=multiplier;
      wentlow=false;
    }
  }
  if(en==0 && en2==0)
    wentlow=true;
  if(ctr++>0xfffe){
    if(ctr2++>0xcffe){
      ctr2=0;    
      // Send value to HID object
      pendant.SetX(encoder_value);
      // Repeat with Y pin
      val = analogRead(27);
      val = map(val, 0, 1023, 1, 10);
      pendant.SetY(val);
      multiplier=val;
      pendant.SetButton(0, !digitalRead(2));
      pendant.SetButton(1,!digitalRead(4));
      pendant.send_update();
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
      encoder_value=0;
    }
  }  
  //delay(100);
}

