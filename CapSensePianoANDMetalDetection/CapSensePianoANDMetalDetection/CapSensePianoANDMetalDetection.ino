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
#define MD_PIN                8    // Pin that will read the frequency from the metal detector for EE241
#define EM_PIN                10   // Pin that controls the electromagnet for EE241
#define EM_THRESHOLD          3    // The amount of change in frequency when the metal ball drops in the metal detector for EE241
#define MD_POWER              11   // Powers the Metal detector with arduino 5V pin

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
  pinMode(MD_POWER, OUTPUT);              // Sets the power rail to 5V for the Metal Detector for EE241
  pinMode(MD_PIN, INPUT);                 // We are reading Period from the Metal Detector
  digitalWrite(EM_PIN, HIGH);             // force the Electromagnet high in setup, the project needs it to turn off once the condition is met
  digitalWrite(MD_POWER, HIGH);           // Powers the Metal Detector Circuit for EE241

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
      seqDet(i);                  // Calls State machine to read sequence and perform Metal Detection
      delay(250);
    }
  }
}


// State machine code for sequence detection by Ethan Buttram for EE241 at Cal Poly San Luis Obispo
// This FSM will carry to the next state if the desired key is pressed, otherwise it will start over at state 1: "FIRST"
// Permission to use is granted, with credit in the comments

void seqDet(int key) {
  enum {FIRST, SECOND, THIRD, FOURTH, FIFTH, TURNOFFEM}; // Key sequence is 1-4-2-3-4
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
      if (key == 3) {       //reading for the 4th key and begins waiting for the Metal detector to detect metal and turn off the EM
        digitalWrite(PIANO_SIGNAL_PIN, HIGH);       // Turns on the relay for the strobe light
        unsigned static long tStart;                // Initializes tStart to be passed into turnOffEM() 
        while (true) {                              // Infinite loop to repeatedly call the turnOffEM() so that metal detection is constant
          turnOffEM(tStart);                        // Calls turnOffEM() to begin reading for Metal Detection
        }     
      }
      else {
        state = FIRST;
      }
      break;

    default: state = FIRST;
      break;
  }
}

void turnOffEM(unsigned long timeStart) {
  timeStart = millis();                                         // Starting Millis() to void the first few values of the period. Initial testing showed that the values of period resulted in an exponential decay of values, some of which met the condition to turn off the EM
  int period = pulseIn(MD_PIN, HIGH) + pulseIn(MD_PIN, LOW);    // Sums time high and low to get the period of the wave
  Serial.println(period);                                       // For debugging
  if (period >= 79 and period <= 90 and timeStart > 200) {      // The range of acceptable values found from testing that will be shown when the metal detector has detected a metal object
    digitalWrite(EM_PIN, LOW);                                  // Turned off the Electromagnet
  }
  else {
    digitalWrite(EM_PIN, HIGH);                                 // Mainly for testing, this allowed testing for meeting the condition after 200ms
  }
  delay(25);                                                    // Delay 25ms for stability, also means that the condition will not be met for approx the first 8 values of period.
}
