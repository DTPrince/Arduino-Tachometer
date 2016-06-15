#include <Adafruit_NeoPixel.h>

#define PIN_LED_OUT 6     // dig. PWM out pin for LED control
#define N_LED 60          // number of LEDs per strand. (0-59)
#define PIN_FREQ 2        // Frequency read pin. Pin 2 isn't PWM and can use ArrachInterrupt()

#define R_MASK 0xFF000
#define G_MASK 0xFF00
#define B_MASK 0xFF  //For bit masking purposes

// Side note: 800kHz is a 1.25 us period, so delay times and sampling won't ever really approach that limit (here)
// (update) You know, I say that but the notes on ::setBrightness() make me worry a bit about it. Still, should be fine.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN_LED_OUT, NEO_GRB + NEO_KHZ800); //initialize. RGB, 800kHz data rate

void setup() {
  strip.begin();  //Thrilling.
  strip.show();   // all show default (0,0,0) values. Helps with turning on the board
}

// *.Color(R, G, B) takes numerical RGB values (0-255, brigness) and returns a 32-bit color code
void loop() {
  fadeLoop(strip.Color(255, 0, 0)); // red
  fadeLoop(strip.Color(0, 255, 0)); // green
  fadeLoop(strip.Color(0, 0, 255)); // blue
  fadeLoop(strip.Color(255, 0, 255)); // test
  //fadeLoop(strip.Color(255, 255, 255));
  //fadeLoop(1);
  //fadeLoop(2);
  //fadeLoop(3); //bit magic makes these vestigial
}

// strip.numPixels()+4 because it cannot write to nonexistant LEDs and I need the loop to 
// complete with turning off the i-4 pixel. (and that one needs to be num 59)
// the lib actually takes care of the n > numLED conditional in setPixelColor. Yass.

// strip.setBrightness(n) should be marvelous, actually. ( NEVERMIND, this is supposed to be a setup function -_- )
// I'm only keeping 4 LEDs on at a time so I can power it from USB on my computer. 

// Even if I use this as a tachometer, it'll either be from a 9V whatever battery or the car battery.
// and considering the strip can draw ~3A at full tilt, I imagine a poor little 9V battery would fare poorly.
// I don't fancy killing my more expensive car battery either. I don't know it's operational amperage either, though it's written on top
// The alternator also supplies more or less power depending on revs...
// Might be nice, actually since the power requirements go up as the revs increase.

// These comments should really go in a *.ideas text file.
void fadeLoop(uint32_t rgb) {
  // ugly and unfortunate, but the library doesn't have a great brightness adjusting function that can be called repeatedly
  // update note: I should be able to just handle the color library with bit access to the uint32_t color. (uint32_t)(color >> 16), etc.
  // I can probably ditch this in general by saying (uint8_t)intensity >> rgb * 8 (for a byte) which will take care of positioning.
  /* uint32_t color; // guarantees that color is in scope for later. Might not matter anymore since I removed the ref. pointer in the halving fn call (argument).
  if (rgb == 0xFF) {
    color = strip.Color(255,0,0);
  } else if (rgb == 0x00FF) {
    color = strip.Color(0,255,0);
  } else if (rgb == 0x0000FF) {
    color = strip.Color(0,0,255);
  } else {
    return; // break out of fade loop for invalid color
  } */
  
  for(uint16_t i = 0; i < strip.numPixels()+4; i++) {
    strip.setPixelColor(i, rgb);
    // I still need a way to dim the previous LEDs here. But the color code is a 32-bit compressed code.
    // and nobody on the wide open internet seems to be able to explain it.
    // (I already looked in the lib, it returns: " ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b; "
    // Update: it's just writing the rgb color codes through bit shifting into a 32-bit variable.
    // With a little bit (hah) of evil type management, it could be slimmed down to 24 bits (wrgb vs rgb).
    // I doubt I'll have memeory issues with such things. Still, 3 bytes vs 4 bytes multiplied by at least 60 is quite a bit of s(h)avings.

    //I wanted to loop this but it seems dumb to allocate mem for a variable when I don't need it to do anything different each run.
    strip.setPixelColor(i-1, tst_halvePreviousBrightness(i-1));
    strip.setPixelColor(i-2, tst_halvePreviousBrightness(i-2));
    strip.setPixelColor(i-3, tst_halvePreviousBrightness(i-3));
    //strip.setPixelColor(i-1, (strip.getPixelColor(i-1) >> 1));
    //strip.setPixelColor(i-2, (strip.getPixelColor(i-2) >> 1));
    //strip.setPixelColor(i-3, (strip.getPixelColor(i-3) >> 1));
    strip.setPixelColor(i-4, 0x00, 0x00, 0x00); //set off
                                                //Important to note that since Clear is not called, the pixel will remain active in mem until program concludes.
                                                // this means that if a pixel is called, it will never clear. Only important when thinking of tach.
    strip.show(); // fin
    delay(40);    // 40ms delay. Could be a wide variety of things
  }
  //return
}

// I think I'm going to have to halve the values of the rgb code.
// Originally I was thinking of doing a simple mask+read -> subtract fixed number (they all start at 255, would have been easy)
//  such as (0xFF & rgb) - 0x32. Then write to unint32_t.
// I could use the lib more and call reducePrevBright to setPixelColor.
// ^ then I could just use the same 'return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;' code directly into setPixelColor().
// Yay functional programming!
void halvePreviousBrightness(uint16_t n, uint8_t &colorMask) { // num is 16 bits instead of 8 because there is a 144 LED/m version that the lib supports
  // n = pixel number
  // mask r (0x0000FF) -> halve
  // mask g (0x00FF) -> halve
  // mask b (0xFF) -> halve
  // write final value through shift. Screw the upper byte, who needs white pixels. This ain't no Sharp TV (which uses rgby anyhow).

  // However. This is the first iteration of this and I already know what value is passed.
    // I could make this more functional still and stack another frame to handle. Or just a really long return line.
  uint8_t newColor = (strip.getPixelColor(n) & colorMask) / 2;  //this will make -Wall bitch since it's unused. But that's the life of an unused fn
  
}

// I'm going to try a more general catch-all version (to be used in tach mayhaps) down here because 
// I just realized that to determine the shift in the bytes from only passing an 8-bit color mask,
// I'd have to pass rgb from loop() and multiply the shift (i.e. >> rgb * 8).
// and I'd need another pointer for the function.
// Now all I need it the pixel number! yay!
uint32_t tst_halvePreviousBrightness(uint16_t n) {
  //originally the masks had their own vars to sepparate out the bits with. The I realized I was being stupid and I don't need to allocate for that. Infact, a define would be great.
  // reuse vars because the mask is only needed once
  uint8_t r_mask = (strip.getPixelColor(n) & R_MASK) >> 1; // get the pixel 32-bit color. Read w/ mask. Halve brightness.
  uint8_t g_mask = (strip.getPixelColor(n) & G_MASK) >> 1;
  uint8_t b_mask = (strip.getPixelColor(n) & B_MASK) >> 1; 
                                                  // I could technically do this in the return line.
                                                  // Also, I could probably devise an algorithm to just use binary operators to dim it with a custom mask.
                                                  // might be worth looking into. Maybe.
                                                  // (UPDATE): that's exactly what I did.
                                                  // How on earth could I have forgotten that a bit shift to the right is the ~same as dividing by 2? Like goddamn.

  return ((uint32_t)r_mask << 16) | ((uint32_t)g_mask <<  8) | b_mask;
  // ripped from the lib, just shifts the 8 bits to their respective places in the 32-bit color code. Pretty self-explanatory.
}

// The brightness variable in the lib is rather confusing. 
// The idea of a brightness is simple until you realize it's an independant variable from the pixel pointer array 
// Which is all fun and good. But wait. You want pixels of differing brightness levels? Well shit, this variable only stores one level for the whole string.
// Another interesting note, in the fn/method definition file of the lib, it mentions that the brightness level is a setup-time sloppy bitch
// (they admitt it's a bit lossy, particularly for large changes and low levels) because they did some 8x8 multiplication magic
// to ease the instruction sets on a cycle so the WS2811/WS2812 (the LED+driver chip) timings can play nice.
// This worries me a wee bit because I have a rather large batch of instructions from the halving process and I haven't even put in the colors or tach read.
// Maybe functional was not the way to go.
// I can eak out a large amount of performance by keeping static masks in global. Classic initialization+mem bloat as an instructions trade-off.
// A closure might be a boon as well for returns.
