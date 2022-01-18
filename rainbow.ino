/*
 * Standby: Rainbow cycling around in clockwise direction
 */

void rainbow(uint8_t rate) {
  static uint8_t leftHue = 0;
  static uint8_t rightHue = 0;
  
  fill_rainbow(leds, N_PIXELS, leftHue, 7);
  fill_rainbow(leds+N_PIXELS, N_PIXELS, rightHue, 7);
  
  EVERY_N_MILLISECONDS(20) {
    leftHue = (leftHue + rate) % 255;
    rightHue = (rightHue - rate) % 255;
  }
   
  FastLED.show();
}
