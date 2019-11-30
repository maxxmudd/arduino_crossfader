/*created by krabbencutter*/
/*modified by Madd Maxx */

/* Infinium X1 Optical Fader Mod

   Description: Uses an Arduino to interpret values from an Infinium X1
   Optical Crossfader, and outputs them based on the fader position within
   the selected curve mode.
  -------------------------------------------------------------------------------
   Instructions: Hold the button down while powering on the mixer to
   select the smooth curve mode. For sharp cut mode, power on the mixer
   as usual (default mode).
  -------------------------------------------------------------------------------
   Operation: A button on pin 10 is checked for on/off status during start up.
   Depending on the button state, a curve mode is selected. The X1 then
   transmits MIDI values to the Arduino RX pin. Calculations based on fader
   position are made, and the results are output through pins 9 and 11.
  -------------------------------------------------------------------------------
   Notes: The Infinium X1 wires are labeled from left to right, coming from the
   base of the crossfader (careful, wires swap at connector!):

   Red = GROUND ||  BLACK = RX || BLACK: POWER (3.3V)

   A serial connection must be opened at 31250 baud rate (MIDI) to read the data
   for debugging. The Arduino IDE serial monitor cannot interpret information at
   this rate, so use PuTTY with the serial monitor instead.

   Included in the file folder is an Excel spreadsheet with graphs that display
   the available curve modes and their values. Can be used to calculate other
   curve modes in the future.
*/

/* Update log:
    11/29/19: -Commented out the variable assignments that are always
              -constant. Uncomment these for debugging in PuTTY.
              -Added detailed description, instructions, and method of operation.
              -Created a step to blink the onboard LED 3 times once powered on.
    11/28/19: -Switched variable types to byte, removed cut variables.
              -Created two blocks of code: one for sharp cut and one for smooth
               curve. Only one of them will be executed continuously depending
               on the state of a button press during power on.
              -Created a function for the debugging prints.
              -Created Excel spreadsheet for displaying curve information.
  ------------------------------------------------------------------------------
    To do:
              -Add more crossfader curve options, incorporate switch
               or dial instead of button press to preserve settings.
              -If possible, find a better method for calculating the
               volume levels based on position under smooth curve.
              -Test on Arduino Nano and Beetle.
              -Test on various mixers to compile compatibility list.
              -Cut in adjustment with settings preservation.
  ------------------------------------------------------------------------------
*/

// variables for fader position and volume
byte cfx;
byte volA;
byte volB;

// output pins
byte outA = 9;
byte outB = 11;

// button state for selecting crossfader curve
byte butState = 0;

// The Behringer X1 uses a baud rate of 31250. */
void setup() {
  Serial.begin(31250);            // MIDI baud rate (31250)

  pinMode(LED_BUILTIN, OUTPUT);
  for (byte i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);    // blink the LED 3 times at startup
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

void loop() {
  butState = digitalRead(10);               // check state of button at start

  if (butState == 1) {                      // smooth curve mode
    while (true) {
      if (Serial.available()) {
        cfx = Serial.read();                // get fader position

        if (cfx == 0) {
          analogWrite(9, 0);                // set volume A to off
          analogWrite(11, 252);             // set volume B to max
        } else if (cfx == 127) {
          analogWrite(9, 252);              // set volume A to max
          analogWrite(11, 0);               // set volume B to off
        } else if (cfx < 63 && cfx > 0) {
          // volB = 252;                    // set volume A based on position
          volA = cfx * 4;                   // volume B is set to max
          analogWrite(9, volA);
          analogWrite(11, volB);
        } else if (cfx > 64 && cfx < 127) {
          // volA = 252;
          volB = 256 - (cfx * 4);           // set volume B based on position,
          volB = 252 + volB;                // volume A is set to max
          analogWrite(9, 252);
          analogWrite(11, volB);
        } else {
          // volA = 252;
          // volB = 252;
          analogWrite(9, 252);              // set both volumes to max
          analogWrite(11, 252);
        }
        // debug();
      }
    }
  } else {                        // sharp cut mode (default)
    while (true) {
      if (Serial.available()) {
        cfx = Serial.read();      // get fader position
        if (cfx == 0) {
          // volA = 0;
          // volB = 252;
          analogWrite(9, 0);      // set volume A to off
          analogWrite(11, 252);   // set volume B to max
        }
        else if (cfx == 127) {
          // volA = 252;
          // volB = 0;
          analogWrite(9, 252);    // set volume A to max
          analogWrite(11, 0);     // set volume B to off
        }
        else {
          // volA = 252;
          // volB = 252;
          analogWrite(9, 252);    // set both volumes to max
          analogWrite(11, 252);
        }
        // debug();
      }
    }
  }
}

void debug() {
  //  Note: This function does not work unless variables are assigned values
  //  in the above statements. Uncomment the variable assignments to
  //  display them using PuTTY

  //  Print button state, fader position, and volume levels
  Serial.print(butState, DEC);
  Serial.print("\t"); Serial.println();
  Serial.print(cfx, DEC);
  Serial.print("\t"); Serial.println();
  Serial.print(volA, DEC);
  Serial.print("\t"); Serial.println();
  Serial.print(volB, DEC);
  Serial.println(); Serial.println();
}
