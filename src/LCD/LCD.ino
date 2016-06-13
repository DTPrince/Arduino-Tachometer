// This is--presently--unfinished and untested.

#include<LiquidCrystal.h> //Display lib

#define PIN_FREQ 2; //Using 2 because it can use ArrachInterrupt() and won't eat up a valuable PWM pin.

uint32_t period = 0;  //I just want to make sure I know what these will be if I call them before r+w
uint32_t freq =0;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //shifter to the latter pins to avoid pin 2, which is used for the read-in.

void setup() {
  // initialization checks, probably. Check logic level of signal in, engine running or not, etc.
  pinMode(pin, INPUT);  //reading period from pin(2) via PulseIn
  
  lcd.begin(16,2);  // I don't actually know the dimensions of my screen yet, but these are popular dimensions so why not.
                    // As for why on earth this library is column, row I will never know. Probably due to hardware-interface addressing but w/e. 
}

void loop() {
  // 

  //Might loop the loop and use a known number of pulseIn's to set the real-time duration of the loop.
  period = pulseIn(PIN_FREQ, HIGH, 10000); // read period from high logic. Defaulf of 1 (s) is pretty terrible, I'll have to come up with something.
                                      // 10000 = 10 ms sampling
                                      // Also, I have to make sure that the wave I'm reading is actually at TTL levels. That's slightly important.
  freq = 1/period;  //Might have to check against period == 0 or error handle the black hole.
                    // This bit of code is also likely to disappear or be absorbed into the a functional implementation so I don't actaully have to dedicate memory to it.
                    //  ^ depends on how often I need to call freq for readout.
                    
  LiquidCrystal
