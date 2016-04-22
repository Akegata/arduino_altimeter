#include <Adafruit_BMP085.h>

// LED Skydive Altimeter Sketch by Bodey Marcoccia
// Altitude settings can be modified by changing the four altitude variables near the top of the code.

#include <SFE_BMP180.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

SFE_BMP180 pressure;

#define PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);

uint32_t blue = strip.Color(0, 0, 255);
uint32_t cyan = strip.Color(36, 182, 255);
uint32_t cyan_dim = strip.Color(0, 20, 20);
uint32_t green = strip.Color(0, 255, 0);
uint32_t red = strip.Color(255, 0, 0);
uint32_t violet = strip.Color(109, 36, 255);
uint32_t white_dim = strip.Color(20, 20, 20);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t off = strip.Color(0, 0, 0);

int exitalt = 4000; // Set exit altitude.
int breakalt = 1500; // Set breakoff altitude.
int pullalt = 1000; // Set pull altitude.
int harddeck = 700; // Set hard deck.
int num_leds = 4; // Set number of LEDs

double baseline;

int altinc; // Altitude increments - Distance from exit to breakoff divided by 8 (number of LEDs).

int startup = 0;
int altreached = 0;

int setLEDColors(int nr_leds, int color) {
  for(uint16_t i=0; i<nr_leds; i++) {
      strip.setPixelColor(i, color);
  }
  strip.show();
}

int blinkLEDColors(int nr_leds, int color, int on_time, int off_time) {
  for(uint16_t i=0; i<nr_leds; i++) {
      strip.setPixelColor(i, color);
  }
  strip.show();
  delay(on_time);
  for(uint16_t i=0; i<nr_leds; i++) {
      strip.setPixelColor(i, off);
  }
  strip.show();
  delay(off_time);
}


void setup() {
  Serial.begin(9600);

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else {
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while (1);
  }

  altinc = ((exitalt - breakalt) / 8);

  baseline = getPressure();
  strip.begin();
  strip.show();
}


void loop() {

  double a, P;
  P = getPressure();
  a = pressure.altitude(P, baseline);
  int agl = a;

  if (startup == 0) { // Violet through all LEDs on startup. Sets startup variable to 1.
    setLEDColors(4, violet);
    delay(1000);
    setLEDColors(num_leds, off);
    startup = 1;
  }

  if (altreached == 0 && agl > (breakalt + (altinc * 6))) { // Blinks green five times when approaching altitude.
    for(uint16_t i=0; i<5; i++) {
      setLEDColors(num_leds, green);
      delay(100);
      setLEDColors(num_leds, off);      
    }
    altreached = 1;
  }

  if (altreached == 0 && agl < 1000) { // Blinks green every five seconds before 1,000' AGL.
    blinkLEDColors(num_leds,green,100,5000);
  } 

  if (altreached == 1 && agl > 3500) {  // Above breakoff altitude plus 7 altitude increments.
    setLEDColors(num_leds,blue);
  }
  else if (altreached == 1 && agl < 3500 && agl > 3000) { // Above breakoff altitude plus 6 altitude increments.
    blinkLEDColors(num_leds,blue,1000,1000);
  }
  else if (altreached == 1 && agl < 3000 && agl > 2500) { // Above breakoff altitude plus 5 altitude increments.
    setLEDColors(num_leds,green);
  }
  else if (altreached == 1 && agl < 2500 && agl > 2000) { // Above breakoff altitude plus 4 altitude increments.
    blinkLEDColors(num_leds,green,1000,1000);
  }
  else if (altreached == 1 && agl < 2000 && agl > 1500) { // Above breakoff altitude plus 4 altitude increments.
    setLEDColors(num_leds,yellow);
  }
  else if (altreached == 1 && agl < 1500 && agl > 1000) { // Above breakoff altitude plus 4 altitude increments.
    blinkLEDColors(num_leds,red,1000,1000);
  }
  else if (altreached == 1 && agl < 1000) { // Above breakoff altitude plus 4 altitude increments.
    blinkLEDColors(num_leds,red,300,300);
  }
}




double getPressure() {
  char status;
  double T, P, p0, a;

  status = pressure.startTemperature();
  if (status != 0)
  {

    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          return (P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
