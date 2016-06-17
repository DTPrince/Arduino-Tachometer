// lib to keep in mind: https://github.com/PaulStoffregen/FreqMeasure
// I would rather write my own code but that's oft not best practice.
// This would be done in 30 minutes if I just pulled libs from online.
// And so it was. Problem is that I don't think the tach lead is sending a useable signal.
// Either it has insane amounts of noise (easily choked with an inductor/RF choke as the signal is so low frequency),
// is a sinusoid, or a 3.3V signal. Or whatever voltage it feel like, I've been assuming this signal originates from the ECU
// rather than the alternator just based on location and the snake it split from.

#include <Adafruit_NeoPixel.h>  // pushin pixels.
#include <FreqMeasure.h>        // I might as well just take advantage of a clean implementation from someone who knows what they're on about.
                                // Declares a FreqMeasure object as extern in the header. Which is why it looks like one was never initialized

#define PIN_LED_OUT 6     // dig. PWM out pin for LED control
#define N_LED 60          // number of LEDs per strand. (0-59)
#define PIN_FREQ 8        // Frequency read pin. Digital non-PWM

// Side note: 800kHz is a 1.25 us period, so delay times and sampling won't ever really approach that limit (here)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN_LED_OUT, NEO_GRB + NEO_KHZ800); //initialize. RGB, 800kHz data rate
//side note: I don't actually have to convert to frequency if I don't want to. If I do the conversion outside the code it's the same thing when implemented.
uint64_t sum;
uint8_t i;

void setup() {
  Serial.begin(9600);   // for testing purposes. Should remember to remove this
  FreqMeasure.begin();  // Hop-to!
  strip.begin();        // Thrilling.
  strip.show();         // all show default (0,0,0) values. Helps with turning on the board w/ LEDs connected
}

// Setting things in functions might be nice to make portable code between tach implementations

void loop() {

  //period = pulseIn(PIN_FREQ, HIGH); 
  //It'd be nice to get an average of 3 or so. Maybe maybe not. 
  //at idle that would be ~270ms
  //at highway that would be ~56ms
  //at redline that would be ~30ms

  //might be better to let the count run wild and check the if statement again a time diff with millis().
  // in fact I will do just that
  if (FreqMeasure.available()) {
    sum += FreqMeasure.read();
    uint64_t currentTime = millis();
    i++;
    if ((currentTime - millis()) < 100) { //Just averages the period over a 100ms period. Helps with a dynamic range rather than a count measurement.
    //if (i > 2) {  //testing code
      float frequency = FreqMeasure.countToFrequency(sum / i);  // float is fine, resolution is in 100's anyhow.
      //SEND frequency to color algorithm here. freq is in the second if scope
      uint16_t leadLED = (frequency / 100) - 1; // -1 because they are indexed 0-59. integer type will insure decimals are ignored.
      Serial.println(leadLED);
      test(leadLED, strip.Color(255,0,0));
      
      sum = 0;
      i = 0;
    }
  }

  // Now I'll have to decode period to RPM and associated color+LED.
  // 4 will be lit up at all times based on my dimming function I wrote in NeoPixel-Plaything.
  // 700 rpm idle means even at the bottom it'll be gucci. Off means 0rpm -> no LEDs to show()
  
}

uint32_t determineColor(uint16_t &LEDindex) {
  return strip.Color(0,0,0); //temp
                              // could just package my own colors but what the hell. This is clear in intention
}

void test(uint16_t LEDindex, uint32_t rgb) {
  if (LEDindex > 59)
    return;  //This is mostly for testing. And stray shit.
  strip.setPixelColor(LEDindex, rgb); //set lead pixel
  uint8_t r,g,b;
  //extract colors:
  r = (uint8_t)(rgb >> 16),
  g = (uint8_t)(rgb >> 8),
  b = (uint8_t)rgb;

  strip.setPixelColor(LEDindex-1, r >> 1, g >> 1, b >> 1);  // dims previous
  strip.setPixelColor(LEDindex-2, r >> 2, g >> 2, b >> 2);  // double dims double previous
  strip.setPixelColor(LEDindex-3, r >> 3, g >> 3, b >> 3);  // you get the idea
  for (uint16_t i = LEDindex-4; i > 0; i--) {
    strip.setPixelColor(i, 0x00, 0x00, 0x00);        //set all previous off. There can be some weird jumps in the readings leading to some stranded pixels otherwise
  }
  
  strip.show();
  //delay is built in the the calling loop, no need for it here. show() can go here or in parent.
  // here might reduce weird interrupt calls since this frame has the attention.
}

    // attachInterrupt() is, naturally, an interrupt. delay() is also an interrupt so it cannot happen at the same time.
    // Since, at the time of writing, I am initially planning on using attachInterrupt() to catch pulses, it might fight with the delay()
    // just above. In fact, the more likely issue will be delay() holding back the interrupt and skipping pulses.
    // Evidently, delayMicroseconds() does not use a counter so it should work without calling an ISR (interrupt service routine). 
    // Since an ISR is like a first date and drops everything for you, frequency should be declared volatile since it will change seemingly at random so far as the compiler can tell.
    // thinking: attachInterrupt(digitalPinToInterrupt(PIN_FREQ), ISR-to-call, RISING);
    // RISING means it will interrupt when the pin goes from low->high. I wonder how it would catch sinusoids.
    // Probably would just watch for forward conduction on the transistor. Which would be weird as it would act as a small signal amplifier for the non-linear portion of the IV curve.
    // since the interrupt should only increment freq, it should be an extraordinarily fast interrupt. 
    // I don't know how time will be kept at this moment though. (CLK register?)
    // 
    // One thing I only just realized as I'm perusing time libraries, if I simply increment a tick variable for each rising edge,
    // I'll incremend indefinitely until it stops, and the delta-t will only grow as well leading to either a number larger than the container
    // or an extremely memory-hungry progream. Which isn't great for an arduino.
    // If I were running this on a desktop/laptop, it would be fun to construct a circular queue (maybe even a deque) object that
    // logs the time of the tick and calculate frequency from the time diff between itself and the next node (would require populating before use)
    // Then it would move the the next node and overwrite it, continuing the loop. tail = head.
    
    // millis() diffs is an interesting option worth thinking about, the only problem being that the update would still have to be regular.
    // Which presents a problem because although diffs can be floating wherever (who cares about start/fin when you just want the diff), 
    // triggering a new segment to watch (start/fin pair. Likely start = fin, fin = new <whatever>) would be unreliable. The diff would always have a ~fairly accurate
    // time measurement but the span could not be of a guaranteed length. Maybe that won't matter, who knows. 

    // Napkin math: if an idle is at ~700 RPM, that's 11&2/3rds rps. 11 ticks a second is a pleasantly large number to work with. 
    // This means that 11-12 interrupts would be called every second at idle, or ~20ms tops (probably much less) of pin 2 hogging the Arduino.
    // At 3.2k RPM, that's 53&1/3rd rps. Significant because it is normal to accelerate between those rpm ranges w/ a 4.0L in gears 1-3. 
    // My car is manual so it gives me even more slack in RPM calculations. However, 53 pulses a second is now ~100ms of time (I'm being extremely generous when saying an increment will take 2ms. It's probably less than 1.)
    // At redline: 5.6k RPM = 93&1/3rd rps. 1/5th of a second could be worse, honestly. 
    // If I don't delay strip.show()'s and let it push whenever it wants I wouldn't even have to worry about it. Additionally, the interrupts do not happen in a chunk, they're continuously spread over the second.
    // Lots of cracks that can be filled.
    
    // A better way of looking at it is at idle, there will be an interrupt every ~91ms, and at redline there will be an interrupt every ~11ms. Easily workable.
    // ^ this line is all anyone should read. ^

    // Coding gets people in the habit of abysmal spelling. No compiler would ever have the audacity to tell you your own variables are misspelled. 

    // It's been great lads, here I am reading through a million and one people's projects on creating frequency counters and all of them failed to mention that 
    // pulseIn() is part of the standard library and WORKS JUST DANDY FOR 10us-3 MINUTES. I mean honestly, the work is already done.
    // Granted, it still uses interrupts to take the pulse in so the interrupt (bad) math is still relevant but I mean come on...
    // note: 10us = 10^-5 s = 10^5 s^-1 = 100kHz, 3min = who cares.

    // On color determination...
    // Do I assign every LED a color (maybe with #defs to save space since I know what each LED should be colored anywho) at startup and then pass the leading LED through
    // a modified dimming function to go back 3 places, dimming accordingly? Or do I have it determine the color to send? Probs the first one just thinking of how I'd implement the latter
    // I'd have to modify my dimming function to be quite a bit heavier to account for fetching the color of the LED, splicing it, bit shifting, and then moving on to do the same 3 times over.
    // The fastest way would be to work off of the leading LED but it would lose the nice gradient effect I'm going for. Kinda. It would just make a gradient from the leading.
    // I could also make a very complicated algorithm to color all the LEDs which could be turned on it's head to color the trailing LED's at a lesser intensity.
    // Could be clean if I integrated it well at bit level. Something to think about at any rate...

    // I should probably test a plain colored (red would be efficient, white via (0xFF, 0xFF, 0xFF) would be a power hungry) version before I throw in crazy algorithms to color LEDs, eh?
    // I don't even know if my frequency counter works yet.
    
