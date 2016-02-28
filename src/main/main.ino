
#include<LiquidCrystal.h> //Display lib

int pin = 2; //Using 2 because it can use ArrachInterrupt() and won't eat up a valuable PWM pin.
unsigned double period;
unsigned double freq;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //shifter to the latter pins to avoid pin 2, which is used for the read-in.

void setup() {
  // initialization checks, probably. Check logic level of signal in, engine running or not, etc.
  period = 0; //no floating values, no sir.
  pinMode(pin, INPUT);  //reading period from pin(2) via PulseIn
  
}

void loop() {
  // 

  //Might loop the loop and use a known number of pulseIn's to set the real-time duration of the loop.
  period = pulseIn(pin, HIGH, 10000); // read period from high logic. Defaulf of 1 (s) is pretty terrible, I'll have to come up with something.
                                      // 10000 = 10 ms sampling
                                      // Also, I have to make sure that the wave I'm reading is actually at TTL levels. That's slightly important.
  freq = 1/period;  //Might have to check against period == 0 or error handle the black hole.
                    // This bit of code is also likely to disappear or be absorbed into the a functional implementation so I don't actaully have to dedicate memory to it.
                    //  ^ depends on how often I need to call freq for readout.
                    
  LiquidCrystal
