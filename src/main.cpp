#include <Arduino.h>

char colors[] = {'r', 'g', 'b'};
int PINRED = 6;
int PINGREEN = 5;
int PINBLUE = 3;

int COLORBUTTON = 2;
int MODEBUTTON = 4;
bool MODE = false; // true for ambient mode
int COLORINDEX;
int BRIGHTNESS = 50; // global brightness variable

// rgb color struct
typedef struct rgb {
  int red, green, blue;
} RGB;

RGB COLORS[3];
int COLORPIN[3];
RGB colorInit(int red, int green, int blue);
void rgbOn(RGB rgb);
void crossFade(int pin1, int pin2, int lag);
void fadeOn(int pin, int power);
bool readButtonInput(int button);
void rgbFadeOn(int r, int g, int b);




void setup() {
  // put your setup code here, to run once:
  pinMode(PINRED, OUTPUT);
  pinMode(PINGREEN, OUTPUT);
  pinMode(PINBLUE, OUTPUT);
  Serial.begin(9600);
  pinMode(COLORBUTTON, INPUT); // Color change button
  // initialize color
  COLORINDEX = 0;
  COLORPIN[0] = PINRED;
  COLORPIN[1] = PINGREEN;
  COLORPIN[2] = PINBLUE;
  analogWrite(COLORPIN[0], BRIGHTNESS);
}

void loop() {
  // test rgb on
  int reading = analogRead(A0); // read temperature
  float VOut = reading * (5/1024.0);
  float temp = (VOut - 0.5) * 100;
  Serial.println(temp);
  //Serial.println(COLORS[COLORINDEX].red);
  //Serial.println(COLORS[COLORINDEX].green);
  //Serial.println(COLORS[COLORINDEX].blue);


  //mode 1 : ambient mode
  // change color
  if (readButtonInput(COLORBUTTON) && MODE) {
    int prev_index = COLORINDEX;
    COLORINDEX = (COLORINDEX + 1) % 3;
    crossFade(COLORPIN[prev_index], COLORPIN[COLORINDEX], 5);
  }
  //mode 2 : temperature mode
  if (!MODE) {
    if (temp < 22) {
      rgbOn(colorInit(0,0,255));
    } else if (temp > 25) {
      rgbOn(colorInit(255,0,0));
    } else {
      rgbOn(colorInit(0,255,0));
    }
  }
  delay(200);


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

RGB colorInit(int red, int green, int blue) {
  RGB color;
  color.red = red;
  color.blue = blue;
  color.green = green;
  return color;
}
