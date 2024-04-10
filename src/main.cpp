#include <Arduino.h>
#include "RGBConverter.h"

char colors[] = {'r', 'g', 'b'};
int PINRED = 6;
int PINGREEN = 5;
int PINBLUE = 3;

int COLORBUTTON = 2;
int MODEBUTTON = 13;
bool MODE = false; // true for ambient mode
int COLORINDEX;
int BRIGHTNESS = 50; // global brightness variable
RGBConverter _rgbconverter;
double hsl[3];
byte rgb[3];

// rgb color struct
typedef struct rgb {
  int red, green, blue;
} RGB;

RGB COLORS[3];
int COLORPIN[3];
RGB colorInit(int red, int green, int blue);
void rgbOn(RGB rgb);
void crossFade(int pin1, int pin2, int lag);
void fadeOn(int pin, int power, int lag);
bool readButtonInput(int button);
void rgbFadeOn(int r, int g, int b);
void rgbOff();
float customMap(long x, float in_min, float in_max, float out_min, float out_max);
RGB temp_to_rgb(long hue_large);




void setup() {
  // put your setup code here, to run once:
  pinMode(PINRED, OUTPUT);
  pinMode(PINGREEN, OUTPUT);
  pinMode(PINBLUE, OUTPUT);
  pinMode(MODEBUTTON, INPUT);
  Serial.begin(9600);
  pinMode(COLORBUTTON, INPUT); // Color change button
  // initialize color
  COLORINDEX = 0;
  COLORPIN[0] = PINRED;
  COLORPIN[1] = PINGREEN;
  COLORPIN[2] = PINBLUE;
  analogWrite(COLORPIN[0], BRIGHTNESS);
  hsl[0] = 0.5;
  hsl[1] = 1.0;
  hsl[2] = 0.5;
}

void loop() {
  // test rgb on


  int reading = analogRead(A0); // read temperature
  double VOut = reading * (5/1024.0) * 100;
  // convert reading to hue
  long hue_large = map(VOut, 60, 75, 0, 360);
  float hue = customMap(hue_large,0.0,360.0, 0.4, 0);
  Serial.print("hue: ");
  Serial.println(hue_large);
  Serial.print("VOut: ");
  Serial.println(VOut);
  // double temp = (VOut - 0.5) * 100;

  int photoresistorvalue = analogRead(A1);
  Serial.println(photoresistorvalue);
  photoresistorvalue = constrain(photoresistorvalue, 200, 650);
  BRIGHTNESS = map(photoresistorvalue, 200, 650, 30, 255);

  // convert hsl to rgb
  hsl[0] = hue;
  _rgbconverter.hslToRgb(hsl[0], hsl[1], hsl[2], rgb);
  

  if (readButtonInput(MODEBUTTON)) {
    MODE = !MODE;
    if (MODE) {
      rgbOff();
      COLORINDEX = 0;
      fadeOn(PINRED, BRIGHTNESS, 2);
    }
    Serial.println(MODE);
    delay(300);
  }

  //mode 1 : ambient mode
  // change color
  if (readButtonInput(COLORBUTTON) && MODE) {
    int prev_index = COLORINDEX;
    COLORINDEX = (COLORINDEX + 1) % 3;
    crossFade(COLORPIN[prev_index], COLORPIN[COLORINDEX], 5);
  }

  analogWrite(COLORPIN[COLORINDEX], BRIGHTNESS);
  //mode 2 : temperature mode
  if (!MODE) {
    /*
    if (temp < 22) {
      rgbOn(colorInit(0,0,BRIGHTNESS));
    } else if (temp > 25) {
      rgbOn(colorInit(BRIGHTNESS,0,0));
    } else {
      rgbOn(colorInit(0,BRIGHTNESS,0));
    }
    */
   RGB color = temp_to_rgb(hue_large);
    rgbOn(colorInit(rgb[0], rgb[1], rgb[2]));
    Serial.print(color.red);
    Serial.print(" ");
    Serial.print(color.green);
    Serial.print(" ");
    Serial.print(color.blue);
    Serial.println("");

  }


}
// put function definitions here:

// turn on rgb.
// Arguments : 
// rgb : RGB value
void rgbOn(RGB rgb) {
  analogWrite(PINRED, rgb.red);
  analogWrite(PINGREEN, rgb.green);
  analogWrite(PINBLUE, rgb.blue);
}

// turn on rgb fade
// Arguments : 
// r : red value
// g : green value
// b : blue value
// lag : fade speed
void rgbFadeOn(int r, int g, int b, int lag) {
  int max;
  if (r >= g && r >= b) max = r;
  else if (g >= r && g >= b) max = g;
  else max = b;
  for (int i = 0; i <= max; i++) {
    analogWrite(PINRED, min(i, r));
    analogWrite(PINGREEN, min(i, g));
    analogWrite(PINBLUE, min(i, b));
    delay(lag);
  }
}

// turn on rgb fade
// Arguments :
// pin : pin number
// power : brightness;
// lag : fade speed;
void fadeOn(int pin, int power, int lag) {
  for (int i = 0; i <= power; i++) {
    analogWrite(pin, i);
    delay(lag);
  }
}

// cross fade color
// Arguments :
// pin1 : pin color to fade off
// pin2 : pin color to fade on;
// lag : fade speed;
void crossFade(int pin1, int pin2, int lag) {
  for (int i = 0; i <= BRIGHTNESS; i++) {
    analogWrite(pin1, BRIGHTNESS - i);
    analogWrite(pin2, i);
    delay(lag);
  }
}


bool readButtonInput(int button) {
  if (digitalRead(button) == HIGH) {
    Serial.println("BUTTON PRESSED");
    return true;
  }
  return false;
}

void rgbOff() {
  analogWrite(PINRED, 0);
  analogWrite(PINGREEN, 0);
  analogWrite(PINBLUE, 0);
}

RGB colorInit(int red, int green, int blue) {
  RGB color;
  color.red = red;
  color.blue = blue;
  color.green = green;
  return color;
}

float customMap(long x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

RGB temp_to_rgb(long hue_large) {
  float red = (255 / 360) * hue_large;
  float blue = (-255 / 360) * hue_large;
  return colorInit(red, 125, blue);
}
