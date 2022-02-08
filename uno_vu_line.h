// --------------------------------
// -- VU Meter - Scott's version --
// --------------------------------

//# define LEFT_OUT_PIN 6             // Left channel data out pin to LEDs [6]
//# define RIGHT_OUT_PIN 5            // Right channel data out pin to LEDs [5]
# define LEFT_IN_PIN A5             // Left aux in signal [A5]
//# define RIGHT_IN_PIN PA4            // Right aux in signal [A4]
//# define DEBOUNCE_MS 20             // Number of ms to debounce the button [20]
//# define LONG_PRESS 500             // Number of ms to hold the button to count as long press [500]
# define N_PIXELS (NUM_LEDS/2)                // Number of pixels in each string [24]
# define MAX_MILLIAMPS 600          // Maximum current to draw [500]
# define COLOR_ORDER GRB            // Colour order of LED strip [GRB]
//# define LED_TYPE WS2812B           // LED string type [WS2812B]
# define DC_OFFSET 0                // DC offset in aux signal [0]
# define NOISE 20                   // Noise/hum/interference in aux signal [10]
# define SAMPLES 60                 // Length of buffer for dynamic level adjustment [60]
# define TOP (N_PIXELS + 2)         // Allow dot to go slightly off scale [(N_PIXELS + 2)]
# define PEAK_FALL 20               // Rate of peak falling dot [20]
# define N_PIXELS_HALF (N_PIXELS / 2)
# define PATTERN_TIME 10            // Seconds to show eaach pattern on auto [10]
//# define STEREO false                // If true, L&R channels are independent. If false, both L&R outputs display same data from L audio channel [false]

uint8_t volCountLeft = 0;           // Frame counter for storing past volume data
int volLeft[SAMPLES];               // Collection of prior volume samples
int lvlLeft = 0;                    // Current "dampened" audio level
int minLvlAvgLeft = 0;              // For dynamic adjustment of graph low & high
int maxLvlAvgLeft = 512;

uint8_t myhue = 0;

void vu4(bool is_centered);
void vu5(bool is_centered);
void vu6(bool is_centered);
void vu7(bool show_background);
void vu8();
void vu9();
void vu10();
void balls();
void fire();
void juggle();
void ripple(boolean show_background);
void sinelon();
void twinkle();
void rainbow(uint8_t rate);

// --------------------
// --- Button Stuff ---
// --------------------
int buttonPushCounter = 0;

void nextSoundEffect(int step) {
  buttonPushCounter = (buttonPushCounter + step + 17) % 17;
//  EEPROM.write(1, buttonPushCounter);
}

void copyLeftToRight() {
  for (uint8_t i = 0; i < N_PIXELS; i++) {
    leds[i+N_PIXELS] = leds[i];
  }
}

void runSelectedPattern() {

  // Run selected pattern
  switch (buttonPushCounter) {
    case 0:
      vu4(false);
      copyLeftToRight();
      break;
  
    case 1:
      vu4(true);
      copyLeftToRight();
      break;
  
    case 2:
      vu5(false);
      copyLeftToRight();
      break;
  
    case 3:
      vu5(true);
      copyLeftToRight();
      break;
  
    case 4:
      vu6(false);
      copyLeftToRight();
      break;
      
    case 5:
      vu7(true);
      copyLeftToRight();
      break;
  
    case 6:
      vu8();
      copyLeftToRight();
      break;
  
    case 7:
      vu9();
      break;
  
    case 8:
      vu10();
      break;
    
    case 9:
      vu7(false);
      copyLeftToRight();
      break;
  
    case 10:
      twinkle();
      break;
  
    case 11:
      sinelon();
      break;
  
    case 12:
      balls();
      break;
  
    case 13:
      juggle();
      break;
  
    case 14:
      fire();
      break;
  
    case 15:
      ripple(false);
      break;
  
    case 16:
      rainbow(10);
      break;
  }
}

// ------------------
// -- VU functions --
// ------------------

uint16_t auxReading() {

  uint16_t height = 0;

    int n = analogRead(LEFT_IN_PIN); // Raw reading from left line in
//    Serial.println(n);
    n = abs(n - ANALOG_MAX/2 - DC_OFFSET); // Center on zero
    n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
    lvlLeft = ((lvlLeft * 7) + n) >> 3; // "Dampened" reading else looks twitchy (>>3 is divide by 8)
    volLeft[volCountLeft] = n; // Save sample for dynamic leveling
    volCountLeft = ++volCountLeft % SAMPLES;
    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvlLeft - minLvlAvgLeft) / (long)(maxLvlAvgLeft - minLvlAvgLeft);

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = constrain(height, 0, TOP);
  return height;
}

/*
 * Function for dropping the peak
 */
uint8_t peakLeft=0, peakRight=0;
void dropPeak() {
  
  static uint8_t dotCountLeft, dotCountRight;
 
    if(++dotCountLeft >= PEAK_FALL) { //fall rate 
      if(peakLeft > 0) peakLeft--;
      dotCountLeft = 0;
    }
}

/*
 * Function for averaging the sample readings
 */
void averageReadings() {

  uint16_t minLvl, maxLvl;

  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
    minLvl = maxLvl = volLeft[0];
    for (int i = 1; i < SAMPLES; i++) {
      if (volLeft[i] < minLvl) minLvl = volLeft[i];
      else if (volLeft[i] > maxLvl) maxLvl = volLeft[i];
    }
    if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
    
    minLvlAvgLeft = (minLvlAvgLeft * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvgLeft = (maxLvlAvgLeft * 63 + maxLvl) >> 6; // (fake rolling average)
}
