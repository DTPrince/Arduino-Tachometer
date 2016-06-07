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
In a perfect world, I'd learn how to program an Intel Edison and go the module route with boiler-plate code so I don't have to drag things like a stripped-down raspbian but I barely even know where to being with something like that.
