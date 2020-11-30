#include <FastLED.h>
#define LEDtilewidth 4 // How much width each input controls
#define LEDtileheight 60 // how high each strip is
#define LEDtilehorz 7 // number of matrices arranged horizontally
#define LEDtilevert 1  // how many tiles stacked vertically
#define LEDstrips (LEDtilewidth*LEDtilehorz)
#define LEDper (LEDtileheight*LEDtilevert)
#define LEDtotal (LEDstrips*LEDper)
#include "EQ.h"
#include <BluetoothSerial.h>

BluetoothSerial Bluetooth;

CRGB leds[LEDtotal];
byte hue;

int loopval = 18;

void setup() {
  Serial.begin(250000);
  FastLED.setCorrection(TypicalSMD5050);
  FastLED.setBrightness(10);
  //FastLED.addLeds<7, WS2813, 19, GRB>(leds, LEDtilewidth * LEDtileheight);
  //FastLED.setMaxRefreshRate(180);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  FastLED.clear();
  
  pinMode(ledPin, OUTPUT);
  EQsampletimer = round(1000000 * (1.0 / EQsamplefreq));
  Bluetooth.begin("Starshroud");
  delay(2000);
  Serial.println("0,1,2,3,4,5,6,7,8,9,10,11,12,13");
}

void loop() {
  /*
  Serial.print("analog 2: ");
  Serial.print(analogRead(2));
  Serial.print(", analog 4: ");
  Serial.print(analogRead(4));
  Serial.print(", analog 0: ");
  Serial.print(analogRead(0));
  Serial.print(", analog 35: ");
  Serial.print(analogRead(35));
  Serial.print(", analog 34: ");
  Serial.println(analogRead(34));
  */
  
  extern int loopval;
  //EVERY_N_SECONDS_I(shufloop, STATEshuffleinterval){
  EVERY_N_MILLIS_I(looper, loopval){     
    EQget();
    EQstats();
    EQnoisegate();
    EQbeatDetection();    
    EQbeatBuckets();
    EQbeatBlink();
    //EQprintone();
    EQprintall();
  }
  EVERY_N_SECONDS(2){
    //loopval++;
    looper.setPeriod(loopval);
  }
  
  //EVERY_N_MILLIS(8){
    quadplexor();
    //visualizer();
    //lightup();
  //}
  
  EVERY_N_MILLIS(500){
    random16_add_entropy(analogRead(0));
    //Serial.print("FPS: ");
    //Serial.println(LEDS.getFPS());
    //Serial.print(" Power: ");
    //Serial.println(calculate_unscaled_power_mW(leds, LEDtotal)/5/LEDbright);
  }
}

void quadplexor(){
  for(int i = 0; i < LEDtotal; i++){
    leds[i] = CRGB(0,0,0);
  }
  for(int band = 0; band < EQbins; band++){
    byte z = map(EQscaled[band], 0, LEDper, 0, LEDper/2);
    for(int leng = 0; leng < z; leng++){                // Display as 00 11 22 33 44 55 66 66 55 44 33 22 11 00  CHSV(hue+leng*5-s*7, 255, 255); EQscaled[band]
      leds[XY(LEDstrips/2 -1  -band,   LEDper/2    -2   -leng)] = CHSV(hue + band*5 + leng*5, 255, 255);  // Top left
      leds[XY(LEDstrips/2     +band,   LEDper/2    -2   -leng)] = CHSV(hue + band*5 + leng*5, 255, 255);    // Top right
      leds[XY(LEDstrips/2 -1  -band,   LEDper/2    -1   +leng)] = CHSV(hue + band*5 + leng*5, 255, 255);  // Bottom left
      leds[XY(LEDstrips/2     +band,   LEDper/2    -1   +leng)] = CHSV(hue + band*5 + leng*5, 255, 255);    // Bottom right
    }
    if(z > 0){ //peaks
      leds[XY(LEDstrips/2 -1  -band,   LEDper/2    -1   -z )] = CHSV(255, 0, 255);  // Top left
      leds[XY(LEDstrips/2     +band,   LEDper/2    -1   -z )] = CHSV(255, 0, 255);    // Top right
      leds[XY(LEDstrips/2 -1  -band,   LEDper/2    -2   +z )] = CHSV(255, 0, 255);  // Bottom left
      leds[XY(LEDstrips/2     +band,   LEDper/2    -2   +z )] = CHSV(255, 0, 255);    // Bottom right
    }
  }
  hue++;
  FastLED.show();
}

void visualizer(){
  //FastLED.clear(); 
  for(int i = 0; i < LEDtotal; i++){
    leds[i] = CRGB(0,0,0);
  }
  for(int band = 0; band < EQbins; band++){
    for(int leng = 0; leng < EQscaled[band]; leng++){                // Display as 00 11 22 33 44 55 66 66 55 44 33 22 11 00  CHSV(hue+leng*5-s*7, 255, 255); EQscaled[band]
      leds[XY(LEDstrips/2+band, leng)] = CHSV(hue+leng*5, 255, 255);
      leds[XY(LEDstrips/2-1-band, leng)] = CHSV(hue+leng*5, 255, 255);
    }
  }
  hue++;
  FastLED.show();
}

void lightup(){
  FastLED.clear();
  for (int x = 0; x < LEDstrips; x++){
    for(int y = 0; y < LEDper; y++){
      leds[XY(x,y)] = CHSV(hue+y*80, 255, 255);
    }
  }
  hue++;
  FastLED.show();
}
