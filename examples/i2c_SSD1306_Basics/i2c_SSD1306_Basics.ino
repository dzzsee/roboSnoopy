//***********************************************************************************************
//  This example shows how to use the basic methods of the RoboSnoopy library.
//
//  Hardware: You'll need a breadboard, an arduino nano r3, an I2C oled display with 1306
//  or 1309 chip and some jumper wires.
//
//  RoboSnoopy draws an animated pixel-art Snoopy on the screen instead of the classic
//  robot eyes. He reflects many emotions and every part of his body moves.
//
//  Published in September 2024 by Dennis Hoelscher, FluxGarage (original RoboEyes)
//  Robo Snoopy edition
//
//***********************************************************************************************


#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// create a RoboEyes instance using an Adafruit_SSD1306 display driver
RoboEyes<Adafruit_SSD1306> roboEyes(display);

// For cycling through all Snoopy emotions automatically
unsigned long eventTimer;
byte mood = 0;
const byte moodCycle[10] = {DEFAULT, TIRED, ANGRY, HAPPY, CONFUSED, SURPRISED, SAD, LOVE, DEAD, YAY};


void setup() {
  Serial.begin(9600);

  // Startup OLED Display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C or 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Startup RoboSnoopy
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100); // screen-width, screen-height, max framerate

  // Define some automated Snoopy behaviour
  roboEyes.setAutoblinker(ON, 3, 2); // Snoopy blinks from time to time (bool active, int interval, int variation in full seconds)
  roboEyes.setIdleMode(ON, 2, 2);    // Idle mode: Snoopy wanders around the screen to random positions
  roboEyes.setMood(DEFAULT);         // Set the initial mood expression

  eventTimer = millis();

} // end of setup


void loop() {
 roboEyes.update(); // update Snoopy drawings - don't use delay() here in order to ensure fluid animations

 // Cycle through all 10 Snoopy emotions every 2 seconds
 if(millis() >= eventTimer+2000){
   mood++;
   if(mood >= 10){ mood = 0; }
   roboEyes.setMood(moodCycle[mood]);
   // Sometimes trigger a oneshot animation together with the mood
   if(moodCycle[mood] == HAPPY){ roboEyes.anim_laugh(); }
   if(moodCycle[mood] == CONFUSED){ roboEyes.anim_confused(); }
   if(moodCycle[mood] == SAD){ roboEyes.setSweat(ON); }
   if(moodCycle[mood] == DEFAULT){ roboEyes.setSweat(OFF); }
   eventTimer = millis();
 }

} // end of loop