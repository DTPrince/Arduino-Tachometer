// This is--presently--unfinished and untested.

#include<LiquidCrystal.h> //Display lib
#include<FreqMeasure.h>

#define PIN_FREQ 8; //The lib actually defines this pin. Weird, but whatever. Non-PWM.

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);  // can use whatever I want so long as it isn't pin 8
                                        // (rs=12, enable=11, d4=2, d5=3, d6=4, d7=5)
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
      lcd.setCursor(0,0);
      lcd.print("RPM:");
      lcd.setCursor(0,1);
      lcd.print(frequency);
      
      sum = 0;
      i = 0;
    }
  }
}
