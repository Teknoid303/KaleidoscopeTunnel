#include "FastLED.h"

//---------------------------------------------------------------------------------------------------------------------------
// KaleidoscopeTunnel LEDs - Opensource Version
//---------------------------------------------------------------------------------------------------------------------------
// This is a simplified version of the code I used for my BBT Tiny Whoop gates for a few years, with some changes to
// make it more user friendly for the open-source world. 
// 
// The mainline code for the gates is located at:
//    https://github.com/Teknoid303/BBTRaceGateProject
//
// I am releasing this version of code to be used with the Kaleidoscope Tunnel Project we built with the Test Flite crew in
// November and December of 2019. Many thanks to the guys - Matt, Josh, Stefan, Alex, Noah, and Michael for all their help,
// Jesse Perkins for making this trip possible, and Paul Nurkkala for jumping in and helping out while he was there.
// It was an awesome experience for all.  
//
//---------------------------------------------------------------------------------------------------------------------------
// Functional Details
//---------------------------------------------------------------------------------------------------------------------------
// If you would like to opt for the more detailed version of this design, see the GitHub link above.
//
// When the gate is turned on/plugged in, there is a short 2 second delay. That is changeable in the code if desired.
//
// Sequenced Pattern Operation - When the gate starts up, it is automatically in a "sequenced mode", meaning that the available
// patterns displayed on the gate are in order, from the first pattern to the last, then over again. There is a programmed delay
// of 20 seconds between pattern changes.
//
//---------------------------------------------------------------------------------------------------------------------------
// The Licensing Blah blah blah
//---------------------------------------------------------------------------------------------------------------------------
// Copyrights:
// Original work Copyright (c) 2017, 2018, 2019 Dean Nicholson via BrightBlueTech(BBT) LLC
// Original work Copyright (c) 2019, 2020, Dean Nicholson via Hydra FPV Drone Entertainment Tech
//
// Attributions:
// Parts based on Demo Reel by Mark Kriegsman, December 2014
// "Marquee fun" (v3) pattern by Marc Miller, May 2016
//
// GPLv3 Note: This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// A copy of the GNU General Public License is available at <https://www.gnu.org/licenses/>
//
// Any queries can be addressed to me directly at dean@hydrafpv.com.
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//---------------------------------------------------------------------------------------------------------------------------

FASTLED_USING_NAMESPACE

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

//---------------------------------------------------------------------------------------------------------------------------
// Normal User adjustable code - Here's where you set up the number of LEDs in your strand, your initial brightness, etc.
//---------------------------------------------------------------------------------------------------------------------------
#define NUM_LEDS           110 // Physical number of LEDs in your string
#define SECONDS_PER_PAT    20  // Number of seconds between pattern change in Sequential and Random modes.
#define INIT_BRIGHTNESS    63  // The brightness your gate will power up with, adjustable between 0 and 255. 
//                                63 is 25%, which works well for all whoop cameras in a dark room.
//---------------------------------------------------------------------------------------------------------------------------

// If you have changed your data pin (LED data output), encoder connections, or button pins, change them here.
#define DATA_PIN              6 //D6
#define HEARTBEATLED_PIN     13 //D13 - Built-in LED on NANO board. Allows us a visual reference when the button1 interrupt is executed.

// LED Setup
#define LED_TYPE          WS2811
#define COLOR_ORDER       GRB
#define FRAMES_PER_SECOND  360
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette(CRGB::Black);
// Many different possibilities here. These are accessible in colorpalettes.h in the FastLED library.
// Available: CloudColors_p, LavaColors_p, OceanColors_p, ForestColors_p, RainbowColors_p, RainbowStripeColors_p, PartyColors_p, and HeatColors_p
CRGBPalette16 targetPalette(PartyColors_p);

// Pattern Variables
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

int hbState = HIGH;       // State of the Heartbeat LED

void setup() {
  pinMode(HEARTBEATLED_PIN, OUTPUT);
  digitalWrite(HEARTBEATLED_PIN, hbState);

  delay(2000); // 2 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(INIT_BRIGHTNESS);

  // Set the maximum power allowed for the string - (volts, milliamps)
  // For now, limit to 1.5A
  FastLED.setMaxPowerInVoltsAndMilliamps(5,1500);
} 

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();

// User Changable - You can change the available patterns here
SimplePatternList gPatterns = { gradient_fill, marque_v3, blendme, three_sin_pal, bpm, rainbow, rainbowWithGlitter, confetti, ConstantColorRed, ConstantColorYellow, ConstantColorGreen, ConstantColorCyan, ConstantColorPurple, ConstantColorBlue};

void loop() {

  // This is an active debug feature. As long as the controller is not hung, the on-board heartbeat LED will
  // toggle with every push of the button. If the LED is not toggling, the controller is hung.
  digitalWrite(HEARTBEATLED_PIN, hbState);
  
  gPatterns[gCurrentPatternNumber]();

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; }  // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( SECONDS_PER_PAT ) { nextPattern(); } // change patterns periodically

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

//---------------------------------------------------------------------------------------------------------------------------
// Patterns - Keep associated variables with the patterns for clarity
//---------------------------------------------------------------------------------------------------------------------------

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 120;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void blendme() {
  uint8_t starthue = beatsin8(20, 0, 255);
  uint8_t endhue = beatsin8(35, 0, 255);
  if (starthue < endhue) {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue, 255, 255), CHSV(endhue, 255, 255), FORWARD_HUES); // If we don't have this, the colour fill will flip around
  } else {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue, 255, 255), CHSV(endhue, 255, 255), BACKWARD_HUES);
  }
} // blendme()

// Variables for three_sin/three_sin_pal
uint8_t thisdelay = 20;                                       // A delay value for the sequence(s)
int     wave1 = 0;                                            // Current phase is calculated.
int     wave2 = 0;
int     wave3 = 0;
uint8_t mul1 = 7;                                            // Frequency, thus the distance between waves
uint8_t mul2 = 6;
uint8_t mul3 = 5;

void three_sin_pal() {
  ChangeMe();

  uint8_t maxChanges = 24;

  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    three_sin();                                              // Improved method of using non-blocking delay
  }
}

void three_sin() {

  wave1 += beatsin8(10, -4, 4);
  wave2 += beatsin8(15, -2, 2);
  wave3 += beatsin8(12, -3, 3);

  for (int k = 0; k < NUM_LEDS; k++) {
    uint8_t tmp = sin8(mul1 * k + wave1) + sin8(mul1 * k + wave2) + sin8(mul1 * k + wave3);
    leds[k] = ColorFromPalette(currentPalette, tmp, 255);
  }

} // three_sin()

void ChangeMe() {

  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB u = CHSV( HUE_BLUE, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;

    switch (secondHand) {
      case  0: targetPalette = RainbowColors_p; break;
      case  5: targetPalette = CRGBPalette16( u, u, b, b, p, p, b, b, u, u, b, b, p, p, b, b); break;
      case 10: targetPalette = OceanColors_p; break;
      case 15: targetPalette = CloudColors_p; break;
      case 20: targetPalette = LavaColors_p; break;
      case 25: targetPalette = ForestColors_p; break;
      case 30: targetPalette = PartyColors_p; break;
      case 35: targetPalette = CRGBPalette16( b, b, b, w, b, b, b, w, b, b, b, w, b, b, b, w); break;
      case 40: targetPalette = CRGBPalette16( u, u, u, w, u, u, u, w, u, u, u, w, u, u, u, w); break;
      case 45: targetPalette = CRGBPalette16( u, p, u, w, p, u, u, w, u, g, u, w, u, p, u, w); break;
      case 50: targetPalette = CloudColors_p; break;
      case 55: targetPalette = CRGBPalette16( u, u, u, w, u, u, p, p, u, p, p, p, u, p, p, w); break;
      case 60: break;
    }
  }
} // ChangeMe()

// draws a line that fades between 2 random colors
// TODO:  Add logic to rotate the starting point
void gradient_fill() {

  //  uint8_t hue1 = 60;
  //  uint8_t hue2 = random8(255);
  uint8_t hue1 = random8(255);
  uint8_t hue2 = hue1 + random8(30, 61);

  for ( int i = 0; i < NUM_LEDS; i++) {
    //fill_gradient (leds, 0, CHSV(0, 255, 255), i, CHSV(96, 255, 255), SHORTEST_HUES);
    fill_gradient (leds, 0, CHSV(hue1, 255, 255), i, CHSV(hue2, 255, 255), SHORTEST_HUES);
    delay(15);
    //    FastLED.show();
    //    FastLED.clear();
  }
}

// gradually fill up the strip with random colors
void randomColorFill(uint8_t wait) {
  fadeToBlackBy( leds, NUM_LEDS, 20);
  //  clearStrip();

  for (uint16_t i = 0; i < NUM_LEDS; i++) { // iterate over every LED of the strip
    int r = random(63, 255); // generate a random color
    int g = random(63, 255);
    int b = random(63, 255);

    for (uint16_t j = 0; j < NUM_LEDS - i; j++) { // iterate over every LED of the strip, that hasn't lit up yet
      leds[j - 1] = CHSV(0, 0, 0); // turn previous LED off
      leds[j] = CHSV(r, g, b); // turn current LED on
      FastLED.show(); // apply the colors
      delay(15);
    }
  }
}
//***************************************************************
// Marquee fun (v3)
//  Pixel position down the strip comes from this formula:
//      pos = spacing * (i-1) + spacing
//  i starts at 0 and is incremented by +1 up to NUM_LEDS/spacing.
//
// Marc Miller, May 2016
//***************************************************************
uint8_t spacing = 20;      // Sets pixel spacing. [Use 2 or greater]
uint8_t width = 5;        // Can increase the number of pixels (width) of the chase. [1 or greater]
int8_t advance;           // Stores the advance amount.
uint8_t color;            // Stores a hue color.
boolean fadingTail = 1;   // Add fading tail? [1=true, 0=false]
uint8_t fadeRate = 70;   // How fast to fade out tail. [0-255]
int8_t delta = 1;         // Sets forward or backwards direction amount. (Can be negative.)
#define MARQ_DEBUG  0

void marque_v3() {

  int16_t pos;

  for (uint8_t i = 0; i < (NUM_LEDS / spacing); i++) {
    for (uint8_t w = 0; w < width; w++) {
      pos = (spacing * (i - 1) + spacing + advance + w) % NUM_LEDS;
      color = gHue;

      leds[pos] = CHSV(color, 255, 255);
    }

    if (MARQ_DEBUG == 1) { // Print out lit pixels if DEBUG is true.
      Serial.print(" "); Serial.print(pos);
    }
    delay(10);
  }
  if (MARQ_DEBUG == 1) {
    Serial.println(" ");
  }
  //  FastLED.show();

  // Fade out tail or set back to black for next loop around.
  if (fadingTail == 1) {
    fadeToBlackBy(leds, NUM_LEDS, fadeRate);
  } else {
    for (uint8_t i = 0; i < (NUM_LEDS / spacing); i++) {
      for (uint8_t w = 0; w < width; w++) {
        pos = (spacing * (i - 1) + spacing + advance + w) % NUM_LEDS;
        leds[pos] = CRGB::Black;
      }
    }
  }

  // Advance pixel position down strip, and rollover if needed.
  advance = (advance + delta + NUM_LEDS) % NUM_LEDS;
}

void ConstantColorGreen () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::Green;
  }
}

void ConstantColorRed () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::Red;
  }
}

void ConstantColorYellow () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::Yellow;
  }
}

void ConstantColorBlue () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::Blue;
  }
}

void ConstantColorPink () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::LightCoral;
  }
}

void ConstantColorPurple () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::BlueViolet;
  }
}

void ConstantColorCyan () {
  for (int k = 0; k < NUM_LEDS; k++) {
    leds[k] = CRGB::Cyan;
  }
}
//---------------------------------------------------------------------------------------------------------------------------
// End of Patterns
//---------------------------------------------------------------------------------------------------------------------------
