#include<Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 2
#define PROGRAM_BUTTON_PIN 1
#define COLOR_BUTTON_PIN 13 
#define BRIGHTNESS_PIN 1
#define DELAY_PIN 2

#define NUMPIXELS 16

#define MAX_BRIGHTNESS 255
#define MAX_DELAY 50

#define NUM_PROGRAMS 3
#define NUM_COLORS 7

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//Single color
//Color switch
//Color fade
//Running lights upwards (single light)
//Running lights upwards (double opposing)
//Running lights double spiral
//Random color all
//Random color + on/off
void singleColor();
void switchColors();
void fadeColors();
void randomColors();
void randomColorsOnOff();

typedef void (*Programs)();
Programs programs[] =  {singleColor, switchColors, fadeColors, randomColors, randomColorsOnOff};

int red[]   = {1,0,0,1,0,1,1};
int green[] = {0,1,0,1,1,0,1};
int blue[]  = {0,0,1,0,1,1,1};

int current_program = 0;
int current_color = 0;
int current_red = 1;
int current_green = 1;
int current_blue = 1;
int current_brightness = 0;
int current_delay = MAX_DELAY;
int programstate = 0;
int last_program_button_state = LOW;
int last_color_button_state = LOW;

void setup() {  
  pinMode(PROGRAM_BUTTON_PIN, INPUT);
  pinMode(COLOR_BUTTON_PIN, INPUT);
  pixels.begin();
}

void loop() {
  //read program button
  int button_state = digitalRead(PROGRAM_BUTTON_PIN);
  if(button_state == HIGH && last_program_button_state == LOW) {
    last_program_button_state = HIGH;
    programstate = 0;
    ++current_program %= NUM_PROGRAMS;
  }else if(button_state == LOW) {
    last_program_button_state = LOW;
  }

  //read color button
  button_state = digitalRead(COLOR_BUTTON_PIN);
  if(button_state == HIGH && last_color_button_state == LOW) {
    last_color_button_state = HIGH;
    programstate = 0;
    ++current_color %= NUM_COLORS;
    current_red = red[current_color];
    current_green = green[current_color];
    current_blue = blue[current_color];
  }else if(button_state == LOW) {
    last_color_button_state = LOW;
  }

  //read brightness poti
  current_brightness = (int)(analogRead(BRIGHTNESS_PIN) / 1023.0 * MAX_BRIGHTNESS);
  
  //read speed poti
  current_delay = (int)(analogRead(DELAY_PIN) / 1023.0 * MAX_DELAY);

  //run selected program
  programs[current_program]();

  delay(current_delay);
}

//allways on
void singleColor() {
  for(int i = 0; i < NUMPIXELS; i++) {
    setColor(i, current_red, current_green, current_blue);
  }
  pixels.show();
}

//switch every 10 steps
void switchColors() {
  if(programstate == 0) {
    ++current_color %= NUM_COLORS;
    singleColor();
    programstate = 10;
  }else{
    programstate--;
  }
}

//switch every 1 step
//cycle one color every 10 steps
//full cycle every 70 steps
void fadeColors() {
  if(programstate < 10) {
    current_red = 1;
    current_green = programstate / 10.0;
    current_blue = 0; 
  }
  else if(programstate < 20) {
    current_red = 1 - ((programstate-10) / 10.0);
    current_green = 1;
    current_blue = 0; 
  }
  else if(programstate < 30) {
    current_red = 0;
    current_green = 1;
    current_blue = (programstate - 20) / 10.0; 
  }
  else if(programstate < 40) {
    current_red = 0;
    current_green = 1 - ((programstate - 30) / 10.0);
    current_blue = 1; 
  }
  else if(programstate < 50) {
    current_red = ((programstate - 40) / 10.0);
    current_green = 0;
    current_blue = 1; 
  }
  else if(programstate < 60) {
    current_red = 1;
    current_green = ((programstate - 50) / 10.0);
    current_blue = 1; 
  }
  else if(programstate < 70) {
    current_red = 1;
    current_green = 1 - ((programstate - 60) / 10.0);
    current_blue = 1 - ((programstate - 60) / 10.0); 
  }
  
  for(int i = 0; i < NUMPIXELS; i++) {
     setColor(i, current_green, current_red, current_blue);
  }
  pixels.show();
  
  ++programstate %= 70;
}

//switch every 10 steps
void randomColors() {
  if(programstate == 0) {
    for(int i = 0; i < NUMPIXELS; i++) {
      setColor(i, random(0,1000) / 1000.0, random(0,1000) / 1000.0, random(0,1000) / 1000.0);
    }
    pixels.show();
    programstate = 10;
  }else{
    programstate--;
  }
}

//switch every 10 steps
void randomColorsOnOff() {
  if(programstate == 0) {
    for(int i = 0; i < NUMPIXELS; i++) {
      setColor(i, random(0,1) == 0 ? 0 : (random(0,1000) / 1000.0), random(0,1) == 0 ? 0 : (random(0,1000) / 1000.0), random(0,1) == 0 ? 0 : (random(0,1000) / 1000.0));
    }
    pixels.show();
    programstate = 10;
  }else{
    programstate--;
  }
}

void setColor(int pixel, float red, float green, float blue) {
  pixels.setPixelColor(pixel, green * current_brightness, red * current_brightness, blue * current_brightness);
}
