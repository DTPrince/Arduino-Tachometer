
#include <Adafruit_NeoPixel.h>

#define PIN_LED_OUT 6     // dig. PWM out pin for LED control
#define N_LED 60          // number of LEDs per strand. (0-59)
#define PIN_FREQ 2        // Frequency read pin. Pin 2 isn't PWM and can use ArrachInterrupt()

// Side note: 800kHz is a 1.25 us period, so delay times and sampling won't ever really approach that limit (here)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN_LED_OUT, NEO_GRB + NEO_KHZ800); //initialize. RGB, 800kHz data rate

void setup() {
  strip.begin();  //Thrilling.
  strip.show();   // all show default (0,0,0) values. Helps with turning on the board
}

// *.Color(R, G, B) takes numerical RGB values (0-255, brigness) and returns a 32-bit color code
void loop() {

  phatFadeLoop(strip.Color(255, 0, 0));     // red
  phatFadeLoop(strip.Color(0, 255, 0));     // green
  phatFadeLoop(strip.Color(0, 0, 255));     // blue
  phatFadeLoop(strip.Color(255, 0, 255));   // purp
  phatFadeLoop(strip.Color(255, 255, 0));   // yellow
  phatFadeLoop(strip.Color(0, 255, 255));   // cyan
  phatFadeLoop(strip.Color(255, 255, 255)); // yellow
  
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
    delay(50);    // 40ms delay. Could be a wide variety of things
  }
  //return
}

