//***********************************************************************************************
//  This example shows how to create RoboSnoopy animation sequences without
//  using delay(); - Snoopy sprints to predefined positions, dances, changes
//  its emotions and falls asleep, all in a scripted loop.
//
//  Hardware: an Arduino Nano R3 (or any compatible board), an I2C OLED display
//  with an SSD1306 / SSD1309 chip, and some jumper wires.
//
//  Based on the FluxGarage RoboEyes library examples by Dennis Hoelscher.
//
//***********************************************************************************************

#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <RoboSnoopy.h>
RoboSnoopy<Adafruit_SSD1306> snoopy(display); // create a RoboSnoopy instance


// EVENT TIMER
unsigned long eventTimer; // will save the timestamps
bool event1wasPlayed = 0; // flag variables
bool event2wasPlayed = 0;
bool event3wasPlayed = 0;
bool event4wasPlayed = 0;
bool event5wasPlayed = 0;
bool event6wasPlayed = 0;


void setup() {
  // OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C or 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Startup Snoopy
  snoopy.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60); // screen-width, screen-height, max framerate - 60-100fps are good for smooth animations
  snoopy.setSpeed(45); // pixels per second
  snoopy.setPosition(S); // start bottom center

  eventTimer = millis(); // start event timer from here

} // end of setup


void loop() {
  snoopy.update(); // update Snoopy drawings

  // LOOPED ANIMATION SEQUENCE
  // Walk north-east and become excited when arriving
  if (millis() >= eventTimer + 2000 && event1wasPlayed == 0) {
    event1wasPlayed = 1;
    snoopy.setPosition(NE);
    snoopy.setMood(HAPPY);
  }
  // Arrive: hop and laugh while standing at the top right corner
  if (millis() >= eventTimer + 4200 && event2wasPlayed == 0) {
    event2wasPlayed = 1;
    snoopy.anim_laugh();          // hop up and down
  }
  // Slow down and get tired, then fall asleep
  if (millis() >= eventTimer + 7000 && event3wasPlayed == 0) {
    event3wasPlayed = 1;
    snoopy.setSpeed(18);          // walk slowly
    snoopy.setMood(TIRED);
    snoopy.setPosition(SW);       // stroll to the bottom left corner
  }
  if (millis() >= eventTimer + 9000 && event4wasPlayed == 0) {
    event4wasPlayed = 1;
    snoopy.close();               // fall asleep
  }
  // Wake up angry, run across the screen and shake the head
  if (millis() >= eventTimer + 11000 && event5wasPlayed == 0) {
    event5wasPlayed = 1;
    snoopy.open();
    snoopy.setSpeed(70);          // run!
    snoopy.setMood(ANGRY);
    snoopy.setPosition(E);        // sprint back to the middle right
  }
  if (millis() >= eventTimer + 13000 && event6wasPlayed == 0) {
    event6wasPlayed = 1;
    snoopy.anim_confused();       // shake left and right
  }

  // Do once after defined number of milliseconds, then reset timer and start over
  if (millis() >= eventTimer + 16000) {
    snoopy.setSpeed(45);
    snoopy.setMood(DEFAULT);
    snoopy.setPosition(S);
    // Reset the timer and the event flags to restart the whole "complex animation loop"
    eventTimer = millis();
    event1wasPlayed = 0;
    event2wasPlayed = 0;
    event3wasPlayed = 0;
    event4wasPlayed = 0;
    event5wasPlayed = 0;
    event6wasPlayed = 0;
  }
  // END OF LOOPED ANIMATION SEQUENCE

} // end of main loop