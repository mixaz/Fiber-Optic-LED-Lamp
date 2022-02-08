#include <FastLED.h>
#include <PinButton.h>
#include <EEPROM.h>
//#ifdef ARDUINO_ARCH_AVR
//#include <avr/sleep.h>
//#endif

// define the LEDs
//#define BTN_PIN           7          // Push button on this pin [3]
#define LED_PIN           8          // pin the LEDs are connected to
#define BRIGTH_PLUS_PIN   9
#define BRIGTH_MINUS_PIN  10
#define COLOR_PIN         11
#define PATTERN_PIN       12
#define SOUND_PIN         13

#define ANALOG_MAX  (1024*1)        // (1024*4) for STM32

#define NUM_LEDS 32
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

static CRGB leds[NUM_LEDS];

#include "solid_color_mode.h"
#include "palette_mode.h"
#include "effect_mode.h"

#include "uno_vu_line.h"

//PinButton FunctionButton(BTN_PIN,INPUT_PULLUP);
PinButton brightPlusButton(BRIGTH_PLUS_PIN,INPUT_PULLUP);
PinButton brightMinusButton(BRIGTH_MINUS_PIN,INPUT_PULLUP);
PinButton colorButton(COLOR_PIN,INPUT_PULLUP);
PinButton patternButton(PATTERN_PIN,INPUT_PULLUP);
PinButton soundButton(SOUND_PIN,INPUT_PULLUP);

static int setMode = 0;

static int brightness = 200;

static int effectNum = 0;

//    EEPROM.update(0, setMode);
//    EEPROM.update(1, colorCounter);
//    EEPROM.update(2, paletteCounter);
//    EEPROM.update(3, gCurrentPatternNumber);
//    Serial.println("shooting down");

static void nextEffect(int step) {
  int maxEffects = 11 + ARRAY_SIZE(gPatterns);
  effectNum = (effectNum+step + maxEffects) % maxEffects;
  if (effectNum < 11) {
      setMode = 1;
      paletteCounter = effectNum;
  }
  else {
      setMode = 2;
      gCurrentPatternNumber = effectNum - 11;
  }
}

//#ifdef ARDUINO_ARCH_AVR
//void wakeUp(){
//  Serial.println("Interrupt Fired");
//  sleep_disable();//Disable sleep mode
//  detachInterrupt(0); //Removes the interrupt from pin D2;
//}
//
//void Going_To_Sleep(){
//    sleep_enable();//Enabling sleep mode
//    attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin D2
//    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
//    sleep_cpu();//activating sleep mode
//    Serial.println("just woke up!");//next line of code executed after the interrupt
//}
//#endif

void setup() {
  delay(1000); // power-up safety delay
  
  Serial.begin(115200);
  Serial.println("fastled started");
  
  pinMode(LEFT_IN_PIN, INPUT_PULLUP);
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.setBrightness(brightness);
  FastLED.clear();

  colorCounter = 1;
  
//  EEPROM.update(0, setMode);        // at first run uncomment this for lines for initializing the eeprom data
//  EEPROM.update(1, colorCounter);
//  EEPROM.update(2, paletteCounter);
//  EEPROM.update(3, gCurrentPatternNumber);

//  setMode = EEPROM.read(0);
//  colorCounter = EEPROM.read(1);
//  paletteCounter = EEPROM.read(2);
//  gCurrentPatternNumber = EEPROM.read(3);
}

void fiberLedShow(void) {
  FastLED.show();
  FastLED.delay(2000 / UPDATES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

void loop() {

//  FunctionButton.update();
  brightPlusButton.update();
  brightMinusButton.update();
  colorButton.update();
  patternButton.update();
  soundButton.update();

  if (colorButton.isSingleClick()) {
    if (setMode == 0) {
      colorCounter = (colorCounter+1) % 17;
    }
    else {
      setMode = 0;
    }
  }
  else if (colorButton.isDoubleClick()) {
    if (setMode == 0) {
      colorCounter = (colorCounter-1+17) % 17;
    }
    else {
      setMode = 0;
    }
  }

  if (patternButton.isSingleClick()) {
    if (setMode == 1 || setMode == 2) {
      nextEffect(1);
    }
    else {
      nextEffect(0);
    }
  }
  else if (patternButton.isDoubleClick()) {
    if (setMode == 1 || setMode == 2) {
      nextEffect(-1);
    }
    else {
      nextEffect(0);
    }
  }

  if (soundButton.isSingleClick()) {
    if (setMode == 3) {
      nextSoundEffect(1);
    }
    else {
      setMode = 3;
    }
  }
  else if (soundButton.isDoubleClick()) {
    if (setMode == 3) {
      nextSoundEffect(-1);
    }
    else {
      setMode = 3;
    }
  }

  int newBrightness = brightness;
  if (brightPlusButton.isSingleClick()) { 
    newBrightness += 5; 
  }   
  else if (brightPlusButton.isLongClick()) { 
    newBrightness += 20; 
  }   
  if (brightMinusButton.isSingleClick()) { 
    newBrightness -= 5; 
  }   
  else if (brightMinusButton.isLongClick()) { 
    newBrightness -= 20; 
  }   
  if (newBrightness != brightness) { 
    newBrightness = constrain(newBrightness, 0, 254); 
    Serial.print("brightness: ");
    Serial.println(newBrightness);
    brightness = newBrightness;
  }   

  if (setMode == 0) {
    if (colorCounter % 2 == 0) {
      float breath = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0/255*brightness;
      FastLED.setBrightness(breath);
    }
    else {
      FastLED.setBrightness(brightness);
    }
    fillColor();
    fiberLedShow();
  }
  else if (setMode == 1) {
    FastLED.setBrightness(brightness);
    setPalette();
    FillLEDsFromPaletteColors(paletteCounter);
    fiberLedShow();
  }
  else if (setMode == 2) {
    FastLED.setBrightness(brightness);
    gPatterns[gCurrentPatternNumber]();
    fiberLedShow();
  }
  else if (setMode == 3) {
    FastLED.setBrightness(brightness);
    runSelectedPattern();
  }

  delay(50); 
}
