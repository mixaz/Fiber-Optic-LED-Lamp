/*
 * Standby: Twinkling lights of random colours
 */

void twinkle() {
  
  if (random(25) == 1) {
    uint16_t i = random(N_PIXELS);
    leds[i] = CRGB(random(256), random(256), random(256));
  }
  fadeToBlackBy(leds, N_PIXELS, FADE_RATE);

  if (random(25) == 1) {
    uint16_t i = random(N_PIXELS);
    leds[i+N_PIXELS] = CRGB(random(256), random(256), random(256));
  }
  fadeToBlackBy(leds+N_PIXELS, N_PIXELS, FADE_RATE);
  
  FastLED.show();
  delay(10);
}
