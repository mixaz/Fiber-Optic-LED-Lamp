#include <FastLED.h>
#include <PinButton.h>
#include <EEPROM.h>
//#ifdef ARDUINO_ARCH_AVR
//#include <avr/sleep.h>
//#endif

// define the LEDs
#define LED_PIN 8          // pin the LEDs are connected to
#define BTN_PIN 7          // Push button on this pin [3]

#define ANALOG_MAX  (1024*1)

#define NUM_LEDS 32
#define BRIGHTNESS 160 //maximum brightness
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

CRGB leds[NUM_LEDS];

#include "solid_color_mode.h"
#include "palette_mode.h"
#include "effect_mode.h"

#include "uno_vu_line.h"

PinButton FunctionButton(BTN_PIN,INPUT_PULLUP);

int setMode = 0;

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
  pinMode(LEFT_IN_PIN, INPUT_PULLUP);
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
	FastLED.setBrightness(BRIGHTNESS);
	FastLED.clear();
	Serial.println("fastled started");
//	EEPROM.update(0, setMode);        // at first run uncomment this for lines for initializing the eeprom data
//	EEPROM.update(1, colorCounter);
//	EEPROM.update(2, paletteCounter);
//	EEPROM.update(3, gCurrentPatternNumber);

//	setMode = EEPROM.read(0);
//	colorCounter = EEPROM.read(1);
//	paletteCounter = EEPROM.read(2);
//	gCurrentPatternNumber = EEPROM.read(3);
}

void fiberLedShow(void) {
  FastLED.show();
  FastLED.delay(2000 / UPDATES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

int vvv = 0;

void loop() {

  int vv = digitalRead(BTN_PIN);
  if(vv != vvv) {
    vvv = vv;
    Serial.print(vv);
  }

	FunctionButton.update();

	if (FunctionButton.isSingleClick()) {
    Serial.println("single click");
		if (setMode == 0) {
			colorCounter++;
			if (colorCounter > 17) {
				colorCounter = 0;
			}
     Serial.print("colorCounter: "); Serial.println(colorCounter);
		}
		else if (setMode == 1) {
			paletteCounter++;
			if (paletteCounter > 11) { // adjust if you have more or less than 34 palettes
				paletteCounter = 0;
			}
     Serial.print("paletteCounter: "); Serial.println(paletteCounter);
		}
		else if (setMode == 2) {
			nextPattern();  // Change to the next pattern
		}
    else if (setMode == 3) {
        Serial.print("buttonPushCounter: ");
        Serial.println(buttonPushCounter);
        incrementButtonPushCounter();
    }
    else if (setMode == 4) {
        Serial.println("Single click is not used in this mode");
    }
	}
	else if (FunctionButton.isDoubleClick()) {
    Serial.println("double click");
		setMode++;
		if (setMode > 4) {
			setMode = 0;
		}
    Serial.print("double click, setMode: ");
    Serial.println(setMode);
	}
	else if (FunctionButton.isLongClick()) {
    Serial.println("long click");
		FastLED.clear();
		FastLED.show();
//		EEPROM.update(0, setMode);
//		EEPROM.update(1, colorCounter);
//		EEPROM.update(2, paletteCounter);
//		EEPROM.update(3, gCurrentPatternNumber);
//		Serial.println("shooting down");
//		delay(500);
//		Going_To_Sleep();
//		Serial.println("waking up");
	}

	if (setMode == 0) {
		if (colorCounter % 2 == 0) {
  		float breath = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
  		FastLED.setBrightness(breath);
		}
		else {
			FastLED.setBrightness(BRIGHTNESS);
		}
		ChangeColorPeriodically();
    fiberLedShow();
	}
	else if (setMode == 1) {
		FastLED.setBrightness(BRIGHTNESS);
		ChangePalettePeriodically();
		static uint8_t startIndex = 0;
		startIndex = startIndex + 1;
		FillLEDsFromPaletteColors(startIndex);
    fiberLedShow();
	}
  else if (setMode == 2) {
    gPatterns[gCurrentPatternNumber]();
    fiberLedShow();
  }
  else if (setMode == 3) {
    runSelectedPattern();
  }
  else if (setMode == 4) {
  // Switch pattern if on auto
    EVERY_N_SECONDS(PATTERN_TIME) { 
      incrementButtonPushCounter();
      Serial.print("Auto, pattern ");
      Serial.println(buttonPushCounter); 
    }
    runSelectedPattern();
  }

}
