# Arduino-Tachometer
Finally getting around to that stray tachometer lead hanging around in the engine bay. As I don't have an oscilloscope and this cable goes for a wild, insulated ride, I don't know if it's carrying a sine wave from the alternator, or a square wave from the ECU. If its a sine wave, I'll probably just throw it into a comparator Op-amp. w/ +5V and 0V in. Might also have to get Schmitty but that's easy. I can even build one if I want to spend forever doing any of the following: measuring betas, swap-test emitter resistors to increase base bias of second BJT, or throw a pot in to be done with it.

## Musings

I suspect the arduino will do almost all of the work in terms of circuitry. Power and pull-up/pull-down resistors are just about the only things needed, I think. Powering the arduino and display from a battery seems like a non-ideal way to go about things but I need to see what kind of power the car battery can provide. A serialized LED display will probably not tax the battery too hard but I will probably have to regulate the voltage some how due to the fact that the cold battery provides less voltage than when the alternator is charging it. A voltage follower could prove to be a nice way to transform the battery into a semi-supply. Really I just need to figure out how much SC current the battery can provide, or the internal resistance. I also have no idea what kind of output impedance the battery has, so that makes a voltage follower that much more attractive.

As a side note (one of many), the Edison looks rather interesting. It could be a lot of fun to program it outside of the Arduino IDE as it doesn't need much more than the Intel/Atom Eclipse loaf to hit play. (I suppose the sdk/dev chain and g++ would be just as good but Eclipse would handle lib updates and linking. GDB practice would be nice though.)  I've never done programming for and ADC interface but I mean, come on. A square wave is practically the ideal output for the D side of the ADC and it happens to also be on the A side. Hardware addresses.
Musings on musings. On musings.  
\beta test

## Wave Assumptions

I believe the square wave is a tick for every revolution. But since this - and most - engines are 4-stroke, I need to figure out what a tach normally thinks of as one revolution.

## Initial Code Ideas

Basically for the meat and 'taters of the program, it will probably be a simple loop that watches the square wave inputs' rising edge and increments a counter every time it sees it. Using the internal oscillator as refrence, I can keep track of how long the counter has been incrementing and normalize to a second or minute, and then divide for RPM.
The issue that I will probably come up against is accurately determining how long the loop has been running and sending the serialized output to the display wihtout hanging the program. Fortunately, an '84 Jeep has dogshit acceleration, so I have a lot of slack to work with regarding timing.

## Expansion

I may have to ditch the arduino earlier rather than later in favor of parallel processes/threading for timing+counting and serializing+displaying. I believe this is possible in an R-Pi but I'd have to actually check.
The primary reason for an eventual RPi change is for my sound-system upgrade and wireless charging in the car that turns on when a phone/tablet is placed on or over the pad.
I'm not starting with the RPI because it is overkill at the moment (probably), has steeper power requirements, and is more unfamiliar to me in terms of programming.
In a perfect world, I'd learn how to program an Intel Edison and go the module route with boiler-plate code so I don't have to drag things like a stripped-down raspbian but I barely even know where to begin with something like that.

## Why Are There Multiple Files?

Let me tell you, dear Deter.  
Long story short, I don't have an LCD screen on hand but I *do* have an Adafruit 60LED/m strip.  
So after hashing out a nice way to run the strip from the 5V PWM 6 pin on an Arduino in /NeoPixel-Playtime/src/neopixel_learnin/ and adjust the brightness of the previous pins, I have returned to conquer the tachometer.  
Which is honestly a completely different thing.  

### LCD
Now, in the `LCD` directory I will have the LCD screen implementation. Eventually.  

### LED
In the `LED` directory I will have the LED-strip implementation.

With 60 LEDs per meter, I can have - among other things - the longest tachometer in the world (really puts the *meter* in tacho*meter*, ehh? Got 'em!), and 1 LED equal to 100 RPMs. The 242 a.k.a. 4.0L, his holiness, the king of running on dirt, AMC Straight-6 has a limiter at 5.6k RPMs so I can easily fit the whole range on the strip. I plan on color-coding the LEDs to go from green->yellow->red so it has some reference and swag points.  
(Side note: I'm not sure I'll actually ever run this thing as a daily, that would take a bit o' work to shorten it to less than a meter. Though maybe a meter strip on the dash would be cool. Who knows.)  
If I did ever decide to run this regularly, I would have to sample a nob or something to dim the blindingly-bright LEDs at night. And a proper relay/switch control.

One thing I might consider is leaving lighthouse pixels every 1k rpm or some such to denote how deep in the paint you are. I also just really want to use the pretty blue LED. I could ditch yellow for blue to cut back on one of the active LEDs, which in a perfect world would cut the power requirements in half but I doubt that is true here. I mean, this is the digital world so everything is assumed to be a perfect flower and if anything goes wrong it's a manufacturers defect.

### Binary LED

An idea for trimming down LEDs needed. I imagine this program would have a lot of #defines but be rather simple. with a byte I could easily express the thousands and hundreds places. In fact, I would only need 3 bits for the thousand since I don't need to go above 5 (6 is free).
