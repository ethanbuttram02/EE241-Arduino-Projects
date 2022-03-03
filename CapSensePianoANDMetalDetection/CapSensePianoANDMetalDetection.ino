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

#define COMMON_PIN            2    // The common 'send' pin for all keys
#define BUZZER_PIN            A4   // The output pin for the piezo buzzer
#define NUM_OF_SAMPLES        10   // Higher number whens more delay but more consistent readings
#define CAP_THRESHOLD         150  // Capactive reading that triggers a note (adjust to fit your needs)
#define NUM_OF_KEYS           4    // Number of keys that are on the keyboard
#define PIANO_SIGNAL_PIN      9    // Pin that outputs the signal to the relay for EE241
#define METAL_DETECT_PIN      A5   // Pin that will read the frequency from the metal detector for EE241
#define EM_PIN                10   // Pin that controls the electromagnet for EE241
#define EM_THRESHOLD          300  // The amount of change in frequency when the metal ball drops in the metal detector for EE241


// This macro creates a capacitance "key" sensor object for each key on the piano keyboard:
#define CS(Y) CapacitiveSensor(2, Y)

// Each key corresponds to a note, which are defined here. Uncomment the scale that you want to use:
int notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5}; // C-Major scale
//int notes[]={NOTE_A4,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_A5}; // A-Minor scale
//int notes[]={NOTE_C4,NOTE_DS4,NOTE_F4,NOTE_FS4,NOTE_G4,NOTE_AS4,NOTE_C5,NOTE_DS5}; // C Blues scale

// Defines the pins that the keys are connected to:
CapacitiveSensor keys[] = {CS(3), CS(4), CS(5), CS(6)};

void setup() {
  
  pinMode(BUZZER_PIN, OUTPUT);            // Set the buzzer, piano signals, electromagnet, and metal detector pins as I/O
  pinMode(PIANO_SIGNAL_PIN, OUTPUT);
  pinMode(EM_PIN, OUTPUT);
  pinMode(METAL_DETECT_PIN, INPUT);
  digitalWrite(EM_PIN, HIGH);             // force the Electromagnet high in setup, the project needs it to turn off once the condition is met

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
      tone(BUZZER_PIN, notes[i]); // plays the note according to the key press
      seqDet(i);
      delay(250);
    }

    if ((9800 - getFrequency(A5)) > EM_THRESHOLD) {  //will turn off the pin and electromagnet when the desired change in frequency is met
      digitalWrite(EM_PIN, LOW);
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
        digitalWrite(PIANO_SIGNAL_PIN, HIGH);
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

long getFrequency(int pin) { //function copied from https://forum.arduino.cc/t/read-a-freqency-from-an-analog-pin/129403/21. Property of the author.
#define SAMPLES 4096
  long freq = 0;
  for (unsigned int j = 0; j < SAMPLES; j++) freq += 500000 / pulseIn(pin, HIGH, 250000);
  return freq / SAMPLES;
}
