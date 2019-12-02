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
   position are made, and volume values between 0 and 1023 are output to both
   channels using pins 9 and 11.
  -------------------------------------------------------------------------------
   Notes: The Infinium X1 wires are labeled from left to right, coming from the
   base of the crossfader (careful, wires swap at connector depending on model!):

   Red = GROUND ||  BLACK = RX || BLACK: POWER (3.3V)

   A serial connection must be opened at 31250 baud rate (MIDI) to read incoming
   data from the X1 (for debugging). The Arduino IDE serial monitor cannot
   interpret information at this rate, so use PuTTY's serial monitor instead.

   Included in the file folder is an Excel spreadsheet with graphs that display
   the available curve modes and their values. Can be used to calculate other
   curve modes in the future.

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
int cfx;
int volA;
int volB;

// output pins
byte outA = 9;
byte outB = 11;

// button state for selecting crossfader curve
byte butState = 0;

int standard[128] = { 0, 20, 40, 60, 80, 100, 120, 140, 160, 179, 199, 218, 238, 257, 276, 295, 313,
                      332, 350, 368, 386, 404, 421, 438, 455, 472, 488, 504, 520, 536, 551, 566, 581,
                      595, 610, 623, 637, 650, 663, 676, 688, 700, 712, 723, 734, 745, 756, 766, 776,
                      786, 795, 804, 813, 821, 830, 838, 845, 853, 860, 867, 873, 880, 886, 892, 898,
                      903, 908, 913, 918, 923, 927, 931, 936, 939, 943, 947, 950, 953, 956, 959, 962,
                      965, 967, 970, 972, 974, 976, 978, 980, 982, 983, 985, 986, 988, 989, 990, 991,
                      993, 994, 995, 995, 996, 997, 998, 999, 999, 1000, 1000, 1001, 1002, 1002, 1002,
                      1003, 1003, 1004, 1004, 1004, 1005, 1005, 1005, 1005, 1005, 1006, 1006, 1006,
                      1006, 1008, 1008
                    };

int sigmoid[128] = { 0, 3, 4, 5, 6, 7, 9, 10, 13, 15, 18, 22, 27, 32, 38, 46, 55, 66, 79, 94, 111, 131, 154,
                     181, 211, 244, 281, 321, 364, 409, 456, 504, 552, 599, 644, 687, 727, 764, 797, 827,
                     854, 877, 897, 914, 929, 942, 953, 962, 970, 976, 981, 986, 990, 993, 995, 998, 999,
                     1001, 1002, 1003, 1004, 1005, 1005, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                     1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                     1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                     1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                     1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                     1008, 1008, 1008, 1008, 1008, 1008
                   };

int gaussian[128] = { 0, 2, 6, 14, 25, 39, 56, 75, 97, 122, 148, 176, 205, 236, 268, 301, 335, 369, 402,
                      436, 470, 503, 535, 567, 597, 627, 655, 683, 709, 733, 757, 779, 799, 818, 836,
                      853, 868, 883, 896, 907, 918, 928, 937, 945, 953, 959, 965, 971, 975, 979, 983,
                      986, 989, 992, 994, 996, 998, 999, 1000, 1002, 1003, 1003, 1004, 1008, 1008,
                      1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                      1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                      1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                      1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008,
                      1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008
                    };

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
  // **This will be changed to a dial for
  // additional curve modes**

  if (butState == 1) {                      // transition curve mode
    while (true) {
      if (Serial.available()) {
        cfx = Serial.read();                // get fader position

        if (cfx == 0) {
          analogWrite(9, 0);                // set volume A to off
          analogWrite(11, 1008);            // set volume B to max
        } else if (cfx == 127) {
          analogWrite(9, 1008);             // set volume A to max
          analogWrite(11, 0);               // set volume B to off
        } else if (cfx < 63 && cfx > 0) {
          // volB = 1008;                   // set volume A based on position
          volA = cfx * 8;                   // volume B is set to max
          analogWrite(9, volA);
          analogWrite(11, volB);
        } else if (cfx > 64 && cfx < 127) {
          // volA = 1008;
          volB = 1024 - (cfx * 8);          // set volume B based on position,
          volB = 1008 + volB;               // volume A is set to max
          analogWrite(9, 1008);
          analogWrite(11, volB);
        } else {
          // volA = 1008;
          // volB = 1008;
          analogWrite(9, 1008);              // set both volumes to max
          analogWrite(11, 1008);
        }
        // debug();                          // print values in PuTTY
      }
    }
  } else {                           // skratch mode (default)
    while (true) {
      if (Serial.available()) {
        cfx = Serial.read();         // get fader position
        
        switch (cfx) {               
          case 0:
            // volA = 0;
            // volB = 1008;
            analogWrite(9, 0);       // set volume A to off
            analogWrite(11, 1008);   // set volume B to max
            break;
          case 127:
            // volA = 1008;
            // volB = 0;
            analogWrite(9, 1008);    // set volume A to off
            analogWrite(11, 0);      // set volume B to max
            break;
          default:
            // volA = 1008;
            // volB = 1008;
            analogWrite(9, 1008);    // set both volumes to max
            analogWrite(11, 1008);
            break;
        }
        // debug();                  // print values in PuTTY
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
