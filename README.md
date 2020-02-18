# Kaleidoscope Tunnel
Flite Test is doing a build video on the Kaleidoscope Tunnels that Jesse Perkins and I helped build for their Whoop Course at Edgewater Airpark. This GitHub is in support of that video.

# Electronics Build
**Electronic Parts** - One of each of the items listed below. 
- Arduino Nano
- WS2812B LED Strips (length and quantity depends on the size of your gate)
- Resistor (220-470 ohm) - Optional but suggested
- Various Wire

The connections are fairly simple and can be repeated for as many LED strands you want in your gate.
# ![Schematic](media/KaleidoscopeTunnelCircuit.jpg)

## IMPORTANT - Power Supply Selection
You will need a 5v power supply for this gate. The type and rating of the power supply you select depends on the **TOTAL** number of LEDs that will be in your gate. As a rule of thumb, and only for this display pattern, you will need about 4W of power at 5v for every 120 LEDs you are running. The two gates we built at Flite Test used about 330 LEDs, which required a 12 watt power supply, but we bumped it up to 15 to be sure. Generally, I go higher on my supplies because I know I will change the pattern at some point and push it further.

# Programming
Included here is a very simple code that will be uploaded to each Nano/LED Strip you have at the ends of your mirrors. The pattern changes from one to another in a sequenced pattern with 20 seconds between changes. If you would like to change the switching time, change the following line in your code.

#define SECONDS_PER_PAT    20 

**Number of LEDs** - When installing the LEDs on your Kaleidoscope Tunnel, the number of LEDs may differ from those we used on the tunnels at Flite Test, so be sure and count your LEDs and change the following line as needed:

#define NUM_LEDS         110

**Brightness** - Your Kaleidoscope Tunnel may be in a dark room, which will require the dimming of the LEDs so that they don't become blinding to the FPV camera. Reduce the number below to your liking. The number runs from 0-255, with 255 being the brightest. 63 is 1/4 brightness, 127 is 1/2, and 191 is 3/4.

#define INIT_BRIGHTNESS   63
