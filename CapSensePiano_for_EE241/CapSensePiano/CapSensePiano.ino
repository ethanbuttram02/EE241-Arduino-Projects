/*
  Capacitive-Touch Arduino Keyboard Piano

  Plays piano tones through a buzzer when the user taps touch-sensitive piano "keys"

  Created  18 May 2013
  Modified 23 May 2013
  by Tyler Crumpton and Nicholas Jones

  This code is released to the public domain. For information about the circuit,
  visit the Instructable tutorial at http://www.instructables.com/id/Capacitive-Touch-Arduino-Keyboard-Piano/
*/

#include <CapacitiveSensor.h>
#include "pitches.h"

#define COMMON_PIN      2    // The common 'send' pin for all keys
#define BUZZER_PIN      A4   // The output pin for the piezo buzzer
#define NUM_OF_SAMPLES  10   // Higher number whens more delay but more consistent readings
#define CAP_THRESHOLD   150  // Capactive reading that triggers a note (adjust to fit your needs)
#define NUM_OF_KEYS     4    // Number of keys that are on the keyboard
#define LED_PIN         9

// This macro creates a capacitance "key" sensor object for each key on the piano keyboard:
#define CS(Y) CapacitiveSensor(2, Y)

// Each key corresponds to a note, which are defined here. Uncomment the scale that you want to use:
int notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5}; // C-Major scale
//int notes[]={NOTE_A4,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_A5}; // A-Minor scale
//int notes[]={NOTE_C4,NOTE_DS4,NOTE_F4,NOTE_FS4,NOTE_G4,NOTE_AS4,NOTE_C5,NOTE_DS5}; // C Blues scale

// Defines the pins that the keys are connected to:
CapacitiveSensor keys[] = {CS(3), CS(4), CS(5), CS(6)};

void setup() {
  Serial.begin(9600);
  Serial.println("Sequence is 0-1");
  // Set the buzzer as an output:
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // Turn off autocalibrate on all channels:
  for (int i = 0; i < 4; ++i) {
    keys[i].set_CS_AutocaL_Millis(0xFFFFFFFF);
  }
}



void loop() {
  // Loop through each keys
  for (int i = 0; i < 4; ++i) {
    // If the capacitance reading is greater than the threshold, play a note:
    if (keys[i].capacitiveSensor(NUM_OF_SAMPLES) > CAP_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);
      delay(50);
      digitalWrite(LED_PIN, LOW);
      seqDet(i);
      delay(125);
    }
  }
}

//State machine code for sequence detection by Ethan Buttram for EE241 at Cal Poly San Luis Obispo
//This FSM will carry to the next state if the desired key is pressed, otherwise it will start over at state 1: "FIRST"

void seqDet(int key) {
  enum {FIRST, SECOND, THIRD, FOURTH, FIFTH, STROBE}; //sequence is 1-4-2-3-4
  static int state = FIRST;

  switch (state) {
    case FIRST:
      if (key == 0) {       //reading for the 1st key
        state = SECOND;
      }
      else {
        state = FIRST;
      }
      break;

    case SECOND:
      if (key == 3) {       //reading for the 4th key
        state = THIRD;
      }
      else {
        state = FIRST;
      }
      break;

    case THIRD:
      if (key == 1) {       //reading for the 2nd key
        state = FOURTH;
      }
      else {
        state = FIRST;
      }
      break;

    case FOURTH:
      if (key == 2) {       //reading for the 3rd key
        state = FIFTH;
      }
      else {
        state = FIRST;
      }
      break;

    case FIFTH:
      if (key == 3) {       //reading for the 4th key
        digitalWrite(LED_PIN, HIGH);    //seinding HIGH signal to LED pin to show the sequence was achieved.
        delay(2500);
        digitalWrite(LED_PIN, LOW);
        delay(25);
        state = FIRST;
      }
      else {
        state = FIRST;
      }
      break;

    default: state = FIRST;
      break;
  }
}
