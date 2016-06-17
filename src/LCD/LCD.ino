// This is--presently--unfinished and untested.

#include<LiquidCrystal.h> //Display lib
#include<FreqMeasure.h>

#define PIN_FREQ 8; //The lib actually defines this pin. Weird, but whatever. Non-PWM.

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //shifter to the latter pins to avoid pin 2, which is used for the read-in.
uint64_t sum;
uint8_t i;

void setup() {
  lcd.begin(16,2);  // I don't actually know the dimensions of my screen yet, but these are popular dimensions so why not.
                    // As for why on earth this library is column, row I will never know. Probably due to hardware-interface addressing but w/e.
  Serial.begin(9600);   // for testing purposes. Should remember to remove this
  FreqMeasure.begin();  // Hop-to!
}

void loop() {
  // The old code that used to be here was garbage.
  // Really I always knew it was temporary but like damn
  if (FreqMeasure.available()) {
    sum += FreqMeasure.read();
    uint64_t currentTime = millis();
    i++;
    if ((currentTime - millis()) < 100) { //Just averages the period over a 100ms period. Helps with a dynamic range rather than a count measurement.
    //if (i > 2) {  // testing code. 
                    // Side note: letting it reach 4 would mean a double bit shift rather than the long division route.
                    // I imagine the compiler would optimize this already, doubly so since it's float type but... Eh.
      float frequency = FreqMeasure.countToFrequency(sum / i);  // float is great, I don't care about decimals anywho.
      Serial.println(frequency);
      test(leadLED, strip.Color(255,0,0));
      
      sum = 0;
      i = 0;
    }
  }
}
