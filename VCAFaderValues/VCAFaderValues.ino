/* created by Madd Maxx */

int pinA = A1;  // analog pins for reading output values
int pinB = A2;
int volA;       // stores volume levels
int volB;

/* Decription: Utility for reading the volume values coming from any VCA
   fader. Can be used to test the analog output from the X1's microcontroller
   by running this on another Arduino.

   Instructions: Hook up output pins of the X1 mod to analog pins 1 and 2. Use this
   to read the channel volume values in the serial monitor. To hook up another fader, 
   identify the power and ground pins, then supply power from either from an external
   source or using the Arduino running this code. DO NOT OVERVOLT (3.3V works with 
   Pro X Fade, may differ for others).
*/
void setup() {
  Serial.begin(9600);                                 // start serial connection
}

void loop() {
  volA = analogRead(pinA);                            // read the volume output from
  volB = analogRead(pinB);                            // the crossfader channels
  
  Serial.print("Volume A: "); Serial.println(volA);   // display the output values
  Serial.print("Volume B: "); Serial.println(volB);
  delay(250);                                         // delay 250 ms
}
