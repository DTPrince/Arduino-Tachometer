#include <Adafruit_NeoPixel.h>  // pushin pixels.
#include <FreqMeasure.h>

#define PIN_LED_OUT 6     // dig. PWM out pin for LED control
#define N_LED 60          // number of LEDs per strand. (0-59)
#define PIN_FREQ 8        // Frequency read pin. Digital non-PWM

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN_LED_OUT, NEO_GRB + NEO_KHZ800); //initialize. RGB, 800kHz data rate

uint64_t sum; //sum the perido ticks
uint8_t i;    //iterations. Making that while loop do a for loop's job. pretty much why do... while exists.

void setup() {
  Serial.begin(9600);   // for testing purposes. Should remember to comment this out
  FreqMeasure.begin();  // Hop-to!
  strip.begin();        // Thrilling.
  strip.show();         // all show default (0,0,0) values. Helps with turning on the board w/ LEDs connected
}

// Setting things in functions might be nice to make portable code between tach implementations

void loop() {

  if (FreqMeasure.available()) {
    sum += FreqMeasure.read();
    uint64_t currentTime = millis();
    i++;
    if ((currentTime - millis()) < 100) { //Just averages the period over a 100ms period. Helps with a dynamic range rather than a count measurement.
    //if (i > 2) {  //testing code
      float frequency = FreqMeasure.countToFrequency(sum / i);  // float is fine, resolution is in 100's anyhow.
      //SEND frequency to color algorithm here. freq is in the second if scope
      //decode to 7-bit binary. Keep first/few LEDs free for status lights?
      Serial.println(frequency * 32/3);

      // Since the index is in binary anyhow, I might just be able to do some bit compares to easily turn on/off LED's.
      // As in, I already have a binary version of the indexing, I only need to translate a bit between RPMs->index. Which I did in the non binary version.
      // cast to integer from float -> divide by 100, call it good.

      strip.show(); // this oft goes in the function that determines LED color and index to keep the sets and calls as close as possible.
      // that function doesn't exist right now though
      sum = 0;  //reset for next loop
      i = 0;    // ^^
    }
  }
  //fin
}

uint32_t determineColor(uint16_t &LEDindex) {
  return strip.Color(0,0,0); //temp
                             // could just package my own colors but what the hell. This is clear in intention
}
