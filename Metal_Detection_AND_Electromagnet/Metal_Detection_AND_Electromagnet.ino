//DESIGNED BY ETHAN BUTTRAM AT CAL POLY SAN LUIS OBISPO 3/8/2022
//ALL RIGHTS RESERVED BY AUTHOR AND EDITOR: ETHAN BUTTRAM
//PERMISSION GRANTED TO USE WITH CREDIT

#define MD_PIN 8        //Metal detect signal pin
#define EM_PIN 10       //Electromagnet power pin

#define MD_POWER 11     //Metal deteector power pin

static unsigned long timeStart;    //Global variable initialized for use in the loop() function, static is to reuse the first value of millis() 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                     // Serial initialized for debugging
  pinMode(MD_PIN, INPUT);                 // Metal detect pin is read as input
  pinMode(EM_PIN, OUTPUT);                // Electromagnet pin is an output 5V
  pinMode(MD_POWER, OUTPUT);              // Metal detector needs 5V power
  digitalWrite(EM_PIN, HIGH);             // Electromagnet needs 5V power
  digitalWrite(MD_POWER, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  timeStart = millis();                                         // Starting Millis() to void the first few values of the period. Initial testing showed that the values of period resulted in an exponential decay of values, some of which met the condition to turn off the EM
  int period = pulseIn(MD_PIN, HIGH) + pulseIn(MD_PIN, LOW);    // Sums time high and low to get the period of the wave
  Serial.println(period);                                       // For debugging
  if (period >= 98 and timeStart > 200) {         // The range of acceptable values found from testing that will be shown when the metal detector has detected a metal object
    digitalWrite(EM_PIN, LOW);                                  // Turned off the Electromagnet
  }
  delay(5);                                                    // Delay 25ms for stability, also means that the condition will not be met for approx the first 8 values of period.
}
