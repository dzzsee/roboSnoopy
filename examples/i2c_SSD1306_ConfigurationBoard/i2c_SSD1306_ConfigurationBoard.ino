//***********************************************************************************************
//  RoboSnoopy configuration board
//
//  Explore all Snoopy emotions, positions and effects using a few pushbuttons,
//  a breadboard, an arduino nano (or compatible board), an I2C SSD1306 OLED display
//  and some jumper wires.
//
//  Buttons:
//   - buttonMood:      cycle through all 10 Snoopy emotions
//   - buttonLaugh:     trigger the laugh animation
//   - buttonConfused:  trigger the confused shake animation
//   - buttonFlicker:   horizontal flicker while pressed
//   - buttonSweat:     toggle animated sweat drops
//   - buttonPosition:  cycle through all 8 predefined screen positions
//   - buttonIdle:      toggle idle mode (Snoopy wandering around the screen)
//   - buttonBlink:     trigger a manual blink
//
//***********************************************************************************************

#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <FluxGarage_RoboEyes.h>
RoboEyes<Adafruit_SSD1306> roboEyes(display);

#include <Pushbutton.h> // Pushbutton library by Pololu, can be found in the Arduino library manager

// Create buttons and define their pins
Pushbutton buttonMood(5);     // Cycle Snoopy emotions
Pushbutton buttonLaugh(6);    // Trigger laugh animation
Pushbutton buttonConfused(7); // Trigger confused animation
Pushbutton buttonFlicker(8);  // Toggle flicker animation
Pushbutton buttonSweat(9);    // Toggle sweat drops
Pushbutton buttonPosition(10);// Cycle screen positions
Pushbutton buttonIdle(11);    // Toggle idle mode (wandering)
Pushbutton buttonBlink(12);   // Trigger manual blink

// Mood and position switches
byte mood = 0;
byte position = 0;

// Name aliases for the emotion constants in a cycle
const byte emotionCycle[10] = {DEFAULT, TIRED, ANGRY, HAPPY, CONFUSED, SURPRISED, SAD, LOVE, DEAD, YAY};
const byte positionCycle[9] = {DEFAULT, N, NE, E, SE, S, SW, W, NW};
byte totalEmotions = 10;
byte totalPositions = 9;


void setup() {

  // Initialize OLED display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C or 0x3D
    for(;;); // Don't proceed, loop forever
  }

  // Startup RoboSnoopy
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100); // screen-width, screen-height, max framerate
  roboEyes.setPosition(DEFAULT); // Snoopy starts centered
  roboEyes.setMood(DEFAULT);

  // Some automatic behaviour for a lively companion
  roboEyes.setAutoblinker(ON, 3, 2); // auto blink every 3 seconds + up to 2 seconds random variation

} // end of setup


void loop() {
  roboEyes.update(); // update Snoopy drawings

  readButtons();
} // end of main loop


// Read all buttons
void readButtons(){

  // CYCLE ALL SNOOPY EMOTIONS
  if (buttonMood.getSingleDebouncedPress()){
    mood++;
    if (mood >= totalEmotions){mood = 0;}
    roboEyes.setMood(emotionCycle[mood]);
  }

  // TRIGGER LAUGH ANIMATION
  if (buttonLaugh.getSingleDebouncedPress()){
    roboEyes.anim_laugh();
  }

  // TRIGGER CONFUSED ANIMATION
  if (buttonConfused.getSingleDebouncedPress()){
    roboEyes.anim_confused();
  }

  // ADD HORIZONTAL FLICKER: TURN ON WHEN PRESSED, TURN OFF WHEN RELEASED
  if (buttonFlicker.getSingleDebouncedPress()){
    roboEyes.setHFlicker(ON, 2);
  }
  if (buttonFlicker.getSingleDebouncedRelease()){
    roboEyes.setHFlicker(OFF);
  }

  // TOGGLE SWEAT DROPS
  if (buttonSweat.getSingleDebouncedPress()){
    roboEyes.setSweat(!roboEyes.sweat);
  }

  // CYCLE THROUGH PREDEFINED SCREEN POSITIONS
  if (buttonPosition.getSingleDebouncedPress()){
    position++;
    if (position >= totalPositions){position = 0;}
    roboEyes.setPosition(positionCycle[position]);
  }

  // TOGGLE IDLE MODE (SNOOPY WANDERING AROUND THE SCREEN)
  if (buttonIdle.getSingleDebouncedPress()){
    roboEyes.setIdleMode(!roboEyes.idle);
  }

  // TRIGGER MANUAL BLINK
  if (buttonBlink.getSingleDebouncedPress()){
    roboEyes.blink();
  }

} // end of readButtons