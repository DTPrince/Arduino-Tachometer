
#include <Adafruit_NeoPixel.h>

#define PIN_LED_OUT 6     // dig. PWM out pin for LED control
#define N_LED 60          // number of LEDs per strand. (0-59)
#define PIN_FREQ 2        // Frequency read pin. Pin 2 isn't PWM and can use ArrachInterrupt()

// Side note: 800kHz is a 1.25 us period, so delay times and sampling won't ever really approach that limit (here)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN_LED_OUT, NEO_GRB + NEO_KHZ800); //initialize. RGB, 800kHz data rate
//side note: I don't actually have to convert to frequency if I don't want to. If I do the conversion outside the code it's the same thing when implemented.
uint64_t period;

void setup() {
  strip.begin();  // Thrilling.
  strip.show();   // all show default (0,0,0) values. Helps with turning on the board w/ LEDs connected
}

// Setting things in functions might be nice to make portable code between tach implementations

// *.Color(R, G, B) takes numerical RGB values (0-255, brigness) and returns a 32-bit color code
void loop() {

  phatFadeLoop(strip.Color(255, 0, 0));     // red
  phatFadeLoop(strip.Color(0, 255, 0));     // green
  phatFadeLoop(strip.Color(0, 0, 255));     // blue
  phatFadeLoop(strip.Color(255, 0, 255));   // purp
  phatFadeLoop(strip.Color(255, 255, 0));   // yellow
  phatFadeLoop(strip.Color(0, 255, 255));   // cyan
  phatFadeLoop(strip.Color(255, 255, 255)); // white

  period = pulseIn(PIN_FREQ, HIGH); 
  //It'd be nice to get an average of 3 or so. Maybe maybe not. 
  //at idle that would be ~270ms
  //at highway that would be ~56ms
  //at redline that would be ~30ms
  // I could write an algorithm to just push the result after x ms. Might be more hassle than needed though.
  uint64_t current_time = millis();
  uint8_t i = 0;
  while (current_time - millis() < 100) {
    period += pulseIn(PIN_FREQ, HIGH);
    i++;
  }
  period = period /i; // make sure decimals don't hate on uint64_t
  // We have an average of a 100ms interval now. 10 updates a second... not too bad.

  // Now I'll have to decode period to RPM and associated color+LED.
  // 4 will be lit up at all times based on my dimming function I wrote in NeoPixel-Plaything.
  // 700 rpm idle means even at the bottom it'll be gucci. Off means 0rpm -> no LEDs to show()
  
}

void phatFadeLoop(uint32_t rgb) {
  // causes a loop of specified color
  // forces start at beginning and goes to end. Specifying pixels would require separating loop from function call.
  // easy to do but not needed.
  uint8_t r, g, b;
  for(uint16_t i = 0; i < strip.numPixels()+4; i++) {
    strip.setPixelColor(i, rgb);
    //extract colors:
    r = (uint8_t)(rgb >> 16),
    g = (uint8_t)(rgb >> 8),
    b = (uint8_t)rgb;
    // Possible bug with crazy colors, the color passed might not be what I want on the previous LEDs.
    // Easy fix would be to call getPixelColor and populate r,g,b with codes and then throw them to setPixelColor()
    // But that would be expensive. Will probably have to come up with something for the tach where it gets more red as the rpms climb.
    strip.setPixelColor(i-1, r >> 1, g >> 1, b >> 1);
    strip.setPixelColor(i-2, r >> 2, g >> 2, b >> 2);
    strip.setPixelColor(i-3, r >> 3, g >> 3, b >> 3);
    strip.setPixelColor(i-4, 0x00, 0x00, 0x00); //set off
                                                //Important to note that since Clear is not called, the pixel will remain active in mem until program concludes.
                                                //this means that if a pixel is called, it will never clear. Only important when thinking of tach.
    strip.show(); // fin
    delay(40);    // 40ms delay. Could be a wide variety of things
    
  }
  //return
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
