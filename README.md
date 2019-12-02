# arduino_crossfader
Modified version of krabbenkutter's Infinium X1 Optical Crossfader Mod

   Description: Uses an Arduino to interpret values from an Infinium X1
   Optical Crossfader, and outputs them based on the fader position within
   the selected curve mode.
   
------------------------------------------------------------------------------

   Instructions: Hold the button down while powering on the mixer to
   select the smooth curve mode. For sharp cut mode, power on the mixer
   as usual (default mode).
   
------------------------------------------------------------------------------

   Operation: A button on pin 10 is checked for on/off status during start up.
   Depending on the button state, a curve mode is selected. The X1 then
   transmits MIDI values to the Arduino RX pin. Calculations based on fader
   position are made, and the results are output through pins 9 and 11.
   
------------------------------------------------------------------------------

   Notes: The Infinium X1 wires are labeled from left to right, coming from the
   base of the crossfader (careful, wires swap at connector!):

   Red = GROUND ||  BLACK = RX || BLACK: POWER (3.3V)

   A serial connection must be opened at 31250 baud rate (MIDI) to read the data
   for debugging. The Arduino IDE serial monitor cannot interpret information at
   this rate, so use PuTTY with the serial monitor instead.

   Included in the file folder is an Excel spreadsheet with graphs that display
   the available curve modes and their values. Can be used to calculate other
   curve modes in the future.
