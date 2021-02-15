#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    60
#define LED_12_OCLOCK 29
#define BRIGHTNESS  255
#define HOURS_LEDS   5
#define MINUTES_LEDS 9
#define SECONDS_LEDS 3

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
CRGB ledsPaletteSeconds[256];
CRGB ledsPaletteMinutes[256];
CRGB ledsPaletteHours[256];

void setup_led(){
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    FastLED.setDither( 0 );
    show_0_15_30_45();
}

void show_0_15_30_45() {
  
    fill_solid( leds, NUM_LEDS, 0); // all black
    leds[(LED_12_OCLOCK + 0 * NUM_LEDS / 60) % NUM_LEDS].setRGB( 255,0,0); // white for 0 minutes (12 o'clock)
    leds[(LED_12_OCLOCK + 15 * NUM_LEDS / 60) % NUM_LEDS].setRGB( 0,255,0);// green for 15 minutes
    leds[(LED_12_OCLOCK + 30 * NUM_LEDS / 60) % NUM_LEDS].setRGB( 0,0,255);// blue for 30 minutes
    leds[(LED_12_OCLOCK + 45 * NUM_LEDS / 60) % NUM_LEDS].setRGB( 128,128,0);// yellow for 45 minutes

    FastLED.show();

    delay(2000);
}

void dim(){
  uint8_t brightness;
  
  switch (settings.dim) {
    case DIMOFF:
      brightness = settings.normalbrightness;
      break;
    case DIMON:
      brightness = settings.dimmedbrightness;
      break;
    case DIMSCHEDULED:
      // [A] start with normal brightness
      brightness = settings.normalbrightness;

      if ((settings.dimstarthour*60 + settings.dimstartminute) < (settings.dimstophour*60 + settings.dimstopminute)) {
        // the dim starting time is BEFORE stopping time (e.g. dim start: 10:15 , dim stop 17:45), i.e. dimming in the day
          if (((hours*60 + minutes) > (settings.dimstarthour*60 + settings.dimstartminute)) && ((hours*60 + minutes) < (settings.dimstophour*60 + settings.dimstopminute))){
            brightness = settings.dimmedbrightness;}
      }
      else { // the dim starting time is AFTER stopping time (e.g. dim start: 17:45 , dim stop 10:15), i.e. dimming in the night
          if (((hours*60 + minutes) > (settings.dimstarthour*60 + settings.dimstartminute)) || ((hours*60 + minutes) < (settings.dimstophour*60 + settings.dimstopminute))){
            brightness = settings.dimmedbrightness;}
      }
      break;
  }

  
  FastLED.setBrightness(  brightness );
}

void loop_led(){
  dim();
  paint_background();
  paint_minutes();
  paint_hours();
  if (settings.displayseconds) {
    paint_seconds();
  }
  FastLED.show();
  //Serial.println("loop led");
}

void paint_background(){
  fill_solid( leds, NUM_LEDS, settings.backgroundcolor);
}

void paint_hours(){
  int colorLedHours = LED_12_OCLOCK + hours * NUM_LEDS / 12 + minutes * NUM_LEDS / 12 / 60 - (HOURS_LEDS-1)/2;
  
  for( int i = 0; i < HOURS_LEDS; i++) {
        leds[(i+colorLedHours) % NUM_LEDS] = blend(leds[(i+colorLedHours) % NUM_LEDS], settings.hourcolor, 255);
    }
}

void paint_minutes(){
  int colorLedMinutes = LED_12_OCLOCK + minutes * NUM_LEDS / 60 - (MINUTES_LEDS-1)/2;
  
  for( int i = 0; i < MINUTES_LEDS; i++) {
        leds[(i+colorLedMinutes) % NUM_LEDS] = blend(leds[(i+colorLedMinutes) % NUM_LEDS], settings.minutecolor, 255);
    }
}

void paint_seconds(){
  int colorLedSeconds = LED_12_OCLOCK + seconds * NUM_LEDS / 60 - (SECONDS_LEDS-1)/2;
//  Serial.println(colorLedSeconds);
  
  for( int i = 0; i < SECONDS_LEDS; i++) {
        leds[(i+colorLedSeconds) % NUM_LEDS] = settings.secondcolor;
    }
}
