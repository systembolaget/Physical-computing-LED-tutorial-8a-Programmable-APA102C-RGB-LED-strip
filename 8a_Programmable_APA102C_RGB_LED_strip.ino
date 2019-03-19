// Tutorial 8a. Programmable APA102C RGB LED strip

// Main parts: Adafruit Metro Mini, APA102C strip 144 x RGB LED

// Library required to drive RGB LEDs; use the latest version
#include "FastLED.h"

// Variables that remain constant
const byte pinData = 4; // Digital output pin to LED strip
const byte pinClock = 3;  // Digital output pin to LED strip
const byte numLeds = 144; // Number of LEDs
struct CRGB leds[numLeds]; // Declare an array that stores each LED's data
// Define gradient palette(s) for the paletteTransition() function. See
// https://github.com/FastLED/FastLED/wiki/Gradient-color-palettes and
// http://fastled.io/tools/paletteknife how to create and use gradient
// palettes. One could switch between palettes with a momentary switch,
// a sensor's input, or IoT data
DEFINE_GRADIENT_PALETTE(dummyPalette)
{
    0,   0,   0, 255, // Blue
  128,   0, 255,   0, // Green
  255, 255,   0,   0  // Red
};
DEFINE_GRADIENT_PALETTE(auroraBorealis) {
    0,   0, 255,  28,
  128, 255,   3,  22,
  192, 247,   1,   9,
  255,   0, 255,  28
};
DEFINE_GRADIENT_PALETTE(cumulusClouds)
{
    0, 152, 164, 155,
   30, 139, 152, 140,
   64, 126, 141, 128,
   92,  80,  95,  82,
  107,  46,  59,  47,
  114,  74,  88,  71,
  123, 110, 124, 102,
  196,  46,  58,  39,
  255,  11,  18,   8
};
// Set the LED strip's overall brightness
byte totalBrightness = 255;

// Variables that can change
CRGBPalette16 activePalette = auroraBorealis; // Select a palette
byte paletteIndex = 0; // Current colour from the selected palette
// Total gradient palette colour transition time in minutes, with 10
// minutes = 2343.75 ms per transition step; 1 hour = 14062.5 ms
const byte transitionDuration = 3;
const byte noiseAmount = 21; // Higher value = more granular noise
// The smaller the difference between the two values, the more subtle the
// noise's brightness variation will be
const byte minBrightness = 24;
const byte maxBrightness = 255;

void setup()
{
  // Initialise the FastLED library with the type of programmable RGB LED
  // used, the digital output pins the LED strip is wired to, the array that
  // holds each LED's data, and the number of LEDs in the strip. Note: BGR
  // (not RGB) is the required colour-order for APA102C LEDs
  FastLED.addLeds<APA102, pinData, pinClock, BGR>(leds, numLeds);
}

void loop()
{
  // Render the effect into the array leds
  paletteTransition();

  // Oscillate the overall brightness at a specified BPM, within a range
  FastLED.setBrightness(beatsin8(4, 43, 255));

  // Display all LED's data (= illuminate the LED strip)
  FastLED.show();
}

void paletteTransition()
{
  // Calculate gradient palette transition steps (in milliseconds)
  static const float interval = ((float)(transitionDuration * 60) / 256) * 1000;

  // Declare a variable that stores the noise (= LED brightness) oscillation
  // speed
  static uint32_t dist;

  // Go through all LEDs in the leds array
  for (int i = 0; i < numLeds; i++)
  {

    // Calculate a Simplex Noise value
    int noise = inoise8(dist - i * noiseAmount, dist + i * noiseAmount) % 255;

    // Scale the noise value into a 0 - 255 brighntess value range
    byte brightness = map(noise, 0, 255, minBrightness, maxBrightness);

    // Write a colour fetched from the gradient palette, its brightness
    // modified by the noise, to the indexed LED
    leds[i] = ColorFromPalette(activePalette, paletteIndex, brightness, LINEARBLEND);
  }

  // Oscillate the noise's speed at a specified BPM, within a range
  dist += beatsin8(2, 3, 12);

  // With every step, a new interpolated palette colour is fetched.
  // EVERY_N_MILLISECONDS, part of the FastLED library, avoids using
  // delay(), which stops program execution, which means that button
  // presses, sensor readings, etc. would not be detected! Always try
  // to avoid delay() and use millis()(or EVERY_N_MILLISECONDS when
  // using the FastLED library)
  EVERY_N_MILLISECONDS(interval)
  {
    // Select the next colour
    if (paletteIndex < 255)
    {
      paletteIndex++;
    }

    // Wrap around back to zero at the end (= cycle through palette again)
    if (paletteIndex == 255)
    {
      paletteIndex = 0;
    }
  }
}
