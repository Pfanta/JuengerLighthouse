#include<Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 5
#define PROGRAM_BUTTON_PIN 7
#define COLOR_BUTTON_PIN 8
#define BRIGHTNESS_PIN 1
#define DELAY_PIN 2

#define NUMPIXELS 5

#define MAX_BRIGHTNESS 255
#define MAX_DELAY 5000
#define BASE_DELAY 50

#define NUM_PROGRAMS 5
#define NUM_COLORS 7

#define MIN_RANDOM_BRIGHTNESS 0.5

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void readProgramButton();
void readColorButton();
void readBrightnessPoti();
void readDelayPoti();

//Single color
//Color switch
//Color fade
//Running lights (single light upward)
//Running lights (double opposing)
//Running lights double spiral
//Random color all
//Random color + on/off
void lighthouse();
void singleColor();
void switchColors();
void fadeColors();
void randomColors();

typedef void (*Programs)();
Programs programs[] =  {lighthouse, singleColor, switchColors, fadeColors, randomColors};
//Programs programs[] =  {singleColor, switchColors, fadeColors, runningLightSingleUpwards, runningLightDoubleOpposing, runningLightSpiral, randomColors, randomColorsOnOff};

//singleColor                   good
//switchColors                  good
//fadeColors                    good
//runningLightSingleUpwards     flicker
//runningLightDoubleOpposing    good
//runningLightSpiral            unknown
//randomColors                  good
//randomColorsOnOff             good

float red[]   = {1,0,0,1,0,1,1};
float green[] = {0,1,0,1,1,0,1};
float blue[]  = {0,0,1,0,1,1,1};

int current_program = 0;
int current_color = 0;
float current_red[] = {1,1,1,1,1};
float current_green[] = {1,1,1,1,1};
float current_blue[] = {1,1,1,1,1};
int current_brightness = 0;
int current_delay = MAX_DELAY;
int delayedTime = 0;
int selected_color = 0;
int programstate = 0;
int last_program_button_state = LOW;
int last_color_button_state = LOW;

void setup() {  
  pinMode(PROGRAM_BUTTON_PIN, INPUT);
  pinMode(COLOR_BUTTON_PIN, INPUT);
  Serial.begin(115200);
  pixels.begin();
}

void loop() {
  readProgramButton();
  readColorButton();
  readBrightnessPoti();
  readDelayPoti();

  Serial.println(current_brightness);
  Serial.println(current_delay);

  programs[current_program]();
  showPixels();
  
  delay(BASE_DELAY);
  delayedTime += BASE_DELAY;
}

void readProgramButton() {
  int button_state = digitalRead(PROGRAM_BUTTON_PIN);
  if(button_state == HIGH && last_program_button_state == LOW) {
    last_program_button_state = HIGH;
    programstate = 0;
    ++current_program %= NUM_PROGRAMS;
    delayedTime = MAX_DELAY;
  }else if(button_state == LOW) {
    last_program_button_state = LOW;
  }
}

void readColorButton() {
  int button_state = digitalRead(COLOR_BUTTON_PIN);
  if(button_state == HIGH && last_color_button_state == LOW) {
    last_color_button_state = HIGH;
    ++selected_color %= NUM_COLORS;
    current_color = selected_color;
    delayedTime = MAX_DELAY;
  }else if(button_state == LOW) {
    last_color_button_state = LOW;
  }
}

void readBrightnessPoti() {
  current_brightness = (int)(analogRead(BRIGHTNESS_PIN) / 1023.0 * MAX_BRIGHTNESS);
}

void readDelayPoti() {
  current_delay = (int)(analogRead(DELAY_PIN) / 1023.0 * MAX_DELAY);
}

void lighthouse() {
  selected_color %= 2;
  
  if(selected_color == 0) {
    setColor(0, 1, 0, 0);
    setColor(1, 1, 1, 1);
    setColor(2, 1, 0, 0);
    setColor(3, 1, 1, 1);
    setColor(4, 1, 0, 0);
  }
  else{
    setColor(0, 1, 1, 1);
    setColor(1, 1, 0, 0);
    setColor(2, 1, 1, 1);
    setColor(3, 1, 0, 0);
    setColor(4, 1, 1, 1);
  }
}

void singleColor() {
  setColorAll(red[current_color], green[current_color], blue[current_color]);
}

void switchColors() {
  if(delayedTime >= current_delay) {
    setColorAll(red[current_color], green[current_color], blue[current_color]);
    ++current_color %= NUM_COLORS;
    delayedTime = 0;
  }
}

void fadeColors() {
  if(delayedTime >= (current_delay / 70.0)) {
    float r,g,b;
    if(programstate < 10) {
      r =  1;
      g = programstate / 10.0;
      b = 0;
    } 
    else if(programstate < 20) {
      r = 1 - ((programstate-10) / 10.0);
      g = 1;
      b = 0;
    }
    else if(programstate < 30) {
      r = 0;
      g = 1;
      b = (programstate - 20) / 10.0; 
    }
    else if(programstate < 40) {
      r = 0;
      g = 1 - ((programstate - 30) / 10.0);
      b = 1; 
    }
    else if(programstate < 50) {
      r = ((programstate - 40) / 10.0);
      g = 0;
      b = 1; 
    }
    else if(programstate < 60) {
      r = 1;
      g = ((programstate - 50) / 10.0);
      b = 1; 
    }
    else if(programstate < 70) {
      r = 1;
      g = 1 - ((programstate - 60) / 10.0);
      b = 1 - ((programstate - 60) / 10.0); 
    }
    setColorAll(r,g,b);
    ++programstate %= 70;
    delayedTime = 0;
  }
}

void randomColors() {
  if(delayedTime >= current_delay) {
    for(int i = 0; i < NUMPIXELS; i++) {
      float r,g,b;

      do {
        r = random(0,1000) / 1000.0;
        g = random(0,1000) / 1000.0;
        b = random(0,1000) / 1000.0;
      }while(r < MIN_RANDOM_BRIGHTNESS && g < MIN_RANDOM_BRIGHTNESS && b < MIN_RANDOM_BRIGHTNESS);
      
      setColor(i, r, g, b);
      delayedTime = 0;
    }
  }
}

void setColorAll(float red, float green, float blue) {
  for(int i = 0; i < NUMPIXELS; i++) {
    current_red[i] = red;
    current_green[i] = green;
    current_blue[i] = blue;
  }
}

void setColor(int pixel, float red, float green, float blue) {
  current_red[pixel] = red;
  current_green[pixel] = green;
  current_blue[pixel] = blue;
}

void showPixels() {
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, current_green[i] * current_brightness, current_red[i] * current_brightness, current_blue[i] * current_brightness);
  }
  pixels.show();
}
